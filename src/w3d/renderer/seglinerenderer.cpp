/**
 * @file
 *
 * @author Jonathan Wilson
 *
 * @brief
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "seglinerenderer.h"
#include "dx8indexbuffer.h"
#include "dx8vertexbuffer.h"
#include "dx8wrapper.h"
#include "random.h"
#include "sortingrenderer.h"
#include "v3_rnd.h"
#include "vector3i.h"
#include "vp.h"
#include "w3d.h"
#include <algorithm>

SegLineRendererClass::SegLineRendererClass() :
    m_texture(nullptr),
    m_shader(ShaderClass::s_presetAdditiveSpriteShader),
    m_width(0),
    m_color(1, 1, 1),
    m_opacity(1),
    m_subdivisionLevel(0),
    m_noiseAmplitude(0),
    m_mergeAbortFactor(1.5f),
    m_textureTileFactor(1),
    m_lastUsedSyncTime(W3D::Get_Sync_Time()),
    m_currentUVOffset(0, 0),
    m_UVOffsetDeltaPerMS(0, 0),
    m_bits(DEFAULT_BITS),
    m_vertexCount(0),
    m_vertexBuffer(nullptr)
{
}

SegLineRendererClass::SegLineRendererClass(const SegLineRendererClass &that) :
    m_texture(nullptr),
    m_shader(ShaderClass::s_presetAdditiveSpriteShader),
    m_width(0),
    m_color(1, 1, 1),
    m_opacity(1),
    m_textureTileFactor(1),
    m_subdivisionLevel(0),
    m_noiseAmplitude(0),
    m_mergeAbortFactor(1.5f),
    m_lastUsedSyncTime(that.m_lastUsedSyncTime),
    m_currentUVOffset(0, 0),
    m_UVOffsetDeltaPerMS(0, 0),
    m_bits(DEFAULT_BITS),
    m_vertexCount(0),
    m_vertexBuffer(nullptr)
{
    *this = that;
}

SegLineRendererClass &SegLineRendererClass::operator=(const SegLineRendererClass &that)
{
    if (this != &that) {
        TextureClass *tex = const_cast<TextureClass *>(that.m_texture);
        Ref_Ptr_Set(m_texture, tex);
        m_shader = that.m_shader;
        m_width = that.m_width;
        m_color = that.m_color;
        m_opacity = that.m_opacity;
        m_subdivisionLevel = that.m_subdivisionLevel;
        m_noiseAmplitude = that.m_noiseAmplitude;
        m_mergeAbortFactor = that.m_mergeAbortFactor;
        m_textureTileFactor = that.m_textureTileFactor;
        m_lastUsedSyncTime = that.m_lastUsedSyncTime;
        m_currentUVOffset = that.m_currentUVOffset;
        m_UVOffsetDeltaPerMS = that.m_UVOffsetDeltaPerMS;
        m_bits = that.m_bits;
    }
    return *this;
}

SegLineRendererClass::~SegLineRendererClass()
{
    Ref_Ptr_Release(m_texture);
    delete[] m_vertexBuffer;
}

void SegLineRendererClass::Init(const W3dEmitterLinePropertiesStruct &props)
{
    Set_Merge_Intersections(props.Flags & W3D_ELINE_MERGE_INTERSECTIONS);
    Set_Freeze_Random(props.Flags & W3D_ELINE_FREEZE_RANDOM);
    Set_Disable_Sorting(props.Flags & W3D_ELINE_DISABLE_SORTING);
    Set_End_Caps(props.Flags & W3D_ELINE_END_CAPS);

    switch ((props.Flags & W3D_ELINE_TEXTURE_MAP_MODE_MASK) >> W3D_ELINE_TEXTURE_MAP_MODE_OFFSET) {
        case W3D_ELINE_UNIFORM_WIDTH_TEXTURE_MAP:
            Set_Texture_Mapping_Mode(UNIFORM_WIDTH_TEXTURE_MAP);
            break;
        case W3D_ELINE_UNIFORM_LENGTH_TEXTURE_MAP:
            Set_Texture_Mapping_Mode(UNIFORM_LENGTH_TEXTURE_MAP);
            break;
        case W3D_ELINE_TILED_TEXTURE_MAP:
            Set_Texture_Mapping_Mode(TILED_TEXTURE_MAP);
            break;
    }

    Set_Current_Subdivision_Level(props.SubdivisionLevel);
    Set_Noise_Amplitude(props.NoiseAmplitude);
    Set_Merge_Abort_Factor(props.MergeAbortFactor);
    Set_Texture_Tile_Factor(props.TextureTileFactor);
    Set_UV_Offset_Rate(Vector2(props.UPerSec, props.VPerSec));
}

void SegLineRendererClass::Set_Texture(TextureClass *texture)
{
    Ref_Ptr_Set(m_texture, texture);
}

TextureClass *SegLineRendererClass::Get_Texture() const
{
    if (m_texture) {
        m_texture->Add_Ref();
    }
    return m_texture;
}

void SegLineRendererClass::Set_Current_UV_Offset(const Vector2 &offset)
{
    m_currentUVOffset = offset;
}

void SegLineRendererClass::Set_Texture_Tile_Factor(float factor)
{
    if (factor > 50.0f) {
        captainslog_debug("Texture (%s) Tile Factor (%.2f) too large in SegLineRendererClass!\r\n",
            Get_Texture()->Get_Name().Peek_Buffer(),
            m_textureTileFactor);
        m_textureTileFactor = 50.0f;
    } else {
        if (factor > 0.0f) {
            m_textureTileFactor = factor;
        } else {
            m_textureTileFactor = 0.0f;
        }
    }
}

void SegLineRendererClass::Reset_Line()
{
    m_lastUsedSyncTime = W3D::Get_Sync_Time();
    m_currentUVOffset.Set(0.0f, 0.0f);
}

void SegLineRendererClass::Render(RenderInfoClass &rinfo,
    const Matrix3D &transform,
    unsigned int point_count,
    Vector3 *points,
    const SphereClass &obj_sphere,
    Vector4 *colors)
{
#ifdef BUILD_WITH_D3D8
    struct LineSegmentIntersection
    {
        int point_count;
        int next_segment_id;
        Vector3 direction;
        Vector3 point;
        float texv;
        Vector4 color;
        bool fold;
        bool parallel;
    };

    struct LineSegment
    {
        Vector3 start_plane;
        Vector3 edge_plane[2];
    };

    Matrix4 view;
    DX8Wrapper::Get_Transform(D3DTS_VIEW, view);
    Matrix4 m(true);
    DX8Wrapper::Set_Transform(D3DTS_WORLD, m);
    DX8Wrapper::Set_Transform(D3DTS_VIEW, m);
    Vector2 uv_offset = m_currentUVOffset + (m_UVOffsetDeltaPerMS * (W3D::Get_Sync_Time() - m_lastUsedSyncTime));
    uv_offset.X = uv_offset.X - GameMath::Floor(uv_offset.X);
    uv_offset.Y = uv_offset.Y - GameMath::Floor(uv_offset.Y);
    m_currentUVOffset.Set(uv_offset);
    m_lastUsedSyncTime = W3D::Get_Sync_Time();
    unsigned int chunk_size = (1 << MAX_SEGLINE_SUBDIV_LEVELS >> m_subdivisionLevel) + 1;

    if (chunk_size > point_count) {
        chunk_size = point_count;
    }

    float u_values[2];
    for (unsigned int start_point = 0; start_point < point_count - 1; start_point += chunk_size - 1) {
        unsigned int point_cnt = std::min(chunk_size, point_count - start_point);
        Matrix3D modelview(view[0].X,
            view[0].Y,
            view[0].Z,
            view[0].W,
            view[1].X,
            view[1].Y,
            view[1].Z,
            view[1].W,
            view[2].X,
            view[2].Y,
            view[2].Z,
            view[2].W);

        Vector3 xformed_pts[129];
        float base_tex_v[130];

        VectorProcessorClass::Transform(xformed_pts, &points[start_point], modelview * transform, point_cnt);

        switch (Get_Texture_Mapping_Mode()) {
            case UNIFORM_WIDTH_TEXTURE_MAP:
                for (unsigned int i = 0; i < point_cnt; i++) {
                    base_tex_v[i] = 0;
                }

                u_values[0] = 0;
                u_values[1] = 1;
                break;
            case UNIFORM_LENGTH_TEXTURE_MAP:
                for (unsigned int i = 0; i < point_cnt; i++) {
                    base_tex_v[i] = (float)(start_point + i) * m_textureTileFactor;
                }

                u_values[0] = 0;
                u_values[1] = 0;
                break;
            case TILED_TEXTURE_MAP:
                for (unsigned int i = 0; i < point_cnt; i++) {
                    base_tex_v[i] = (float)(start_point + i) * m_textureTileFactor;
                }

                u_values[0] = 0;
                u_values[1] = 1;
                break;
        }

        unsigned int sub_point_cnt;
        Vector3 xformed_subdiv_pts[129];
        float subdiv_tex_v[130];
        Vector4 subdiv_color_v[129];
        LineSegment segment[130];
        LineSegmentIntersection intersection[130][2];

        if (colors) {
            Subdivision_Util(point_cnt,
                xformed_pts,
                base_tex_v,
                &sub_point_cnt,
                xformed_subdiv_pts,
                subdiv_tex_v,
                &colors[start_point],
                subdiv_color_v);
        } else {
            Subdivision_Util(
                point_cnt, xformed_pts, base_tex_v, &sub_point_cnt, xformed_subdiv_pts, subdiv_tex_v, 0, subdiv_color_v);
        }

        float radius = m_width * 0.5f;
        bool switch_edges = false;

        for (unsigned int sub_point = 1; sub_point < sub_point_cnt; sub_point++) {
            Vector3 prev_point = xformed_subdiv_pts[sub_point - 1];
            Vector3 curr_point = xformed_subdiv_pts[sub_point];

            if (Equal_Within_Epsilon(xformed_subdiv_pts[sub_point - 1], xformed_subdiv_pts[sub_point], GAMEMATH_EPSILON)) {
                curr_point.X = curr_point.X + 0.001f;
            }

            Vector3 plane = Normalize(curr_point - prev_point);
            Vector3 offset =
                Normalize(Vector3::Cross_Product(plane, prev_point + plane * -Vector3::Dot_Product(plane, prev_point)));
            Vector3 top_normal = Normalize(Vector3::Cross_Product(prev_point + radius * offset, plane));
            Vector3 bottom_normal = Normalize(Vector3::Cross_Product(plane, prev_point - radius * offset));
            segment[sub_point].start_plane = plane;
            segment[sub_point].edge_plane[0] = top_normal;
            segment[sub_point].edge_plane[1] = bottom_normal;

            if (sub_point > 1) {
                Vector3 cross1 = Normalize(Vector3::Cross_Product(xformed_subdiv_pts[sub_point - 2], prev_point));
                Vector3 cross2 = Normalize(Vector3::Cross_Product(prev_point, curr_point));

                if (Vector3::Dot_Product(cross1, cross2) >= 0) {
                    intersection[sub_point][0].fold = false;
                    intersection[sub_point][1].fold = false;
                } else {
                    switch_edges = !switch_edges;
                    intersection[sub_point][0].fold = true;
                    intersection[sub_point][1].fold = true;
                }
            }

            if (switch_edges) {
                segment[sub_point].edge_plane[0] = -bottom_normal;
                segment[sub_point].edge_plane[1] = -top_normal;
            }
        }

        enum SegmentEdge
        {
            FIRST_EDGE = 0,
            TOP_EDGE = 0,
            BOTTOM_EDGE = 1,
            MAX_EDGE = 1,
            NUM_EDGES = 2,
        };
        unsigned int num_intersections[NUM_EDGES];
        num_intersections[TOP_EDGE] = sub_point_cnt;
        num_intersections[BOTTOM_EDGE] = sub_point_cnt;

        intersection[0][0].point_count = 0;
        intersection[0][0].next_segment_id = 0;
        intersection[0][0].direction = Vector3(1, 0, 0);
        intersection[0][0].point = Vector3(0, 0, 0);
        intersection[0][0].texv = 0.0f;
        intersection[0][0].color = Vector4(0, 0, 0, 0);
        intersection[0][0].fold = true;
        intersection[0][0].parallel = false;

        intersection[0][1].point_count = 0;
        intersection[0][1].next_segment_id = 0;
        intersection[0][1].direction = Vector3(1, 0, 0);
        intersection[0][1].point = Vector3(0, 0, 0);
        intersection[0][1].texv = 0.0f;
        intersection[0][1].color = Vector4(0, 0, 0, 0);
        intersection[0][1].fold = true;
        intersection[0][1].parallel = false;

        intersection[1][0].point_count = 1;
        intersection[1][0].next_segment_id = 1;
        intersection[1][0].direction = Normalize(xformed_subdiv_pts[0]
            - Vector3::Dot_Product(segment[1].edge_plane[0], xformed_subdiv_pts[0]) * segment[1].edge_plane[0]);
        intersection[1][0].point = xformed_subdiv_pts[0];
        intersection[1][0].texv = subdiv_tex_v[0];
        intersection[1][0].color = subdiv_color_v[0];
        intersection[1][0].fold = true;
        intersection[1][0].parallel = false;

        intersection[1][1].point_count = 1;
        intersection[1][1].next_segment_id = 1;
        intersection[1][1].direction = Normalize(xformed_subdiv_pts[0]
            - Vector3::Dot_Product(segment[1].edge_plane[1], xformed_subdiv_pts[0]) * segment[1].edge_plane[1]);
        intersection[1][1].point = xformed_subdiv_pts[0];
        intersection[1][1].texv = subdiv_tex_v[0];
        intersection[1][1].color = subdiv_color_v[0];
        intersection[1][1].fold = true;
        intersection[1][1].parallel = false;

        Vector3 segdir = Normalize(xformed_subdiv_pts[1] - xformed_subdiv_pts[0]);
        Vector3 start_pl = Normalize(Vector3::Cross_Product(intersection[1][0].direction, intersection[1][1].direction));

        if (Vector3::Dot_Product(segdir, start_pl) <= 0) {
            start_pl = -start_pl;
        }

        segment[0].start_plane = start_pl;
        segment[0].edge_plane[0] = start_pl;
        segment[0].edge_plane[1] = start_pl;
        segment[1].start_plane = start_pl;

        captainslog_dbgassert(sub_point_cnt >= 2, "It is not clear if that is always the case. Investigate if hit.");

        intersection[sub_point_cnt][0].point_count = 1;
        intersection[sub_point_cnt][0].next_segment_id = sub_point_cnt;
        intersection[sub_point_cnt][0].point = xformed_subdiv_pts[sub_point_cnt - 1];
        intersection[sub_point_cnt][0].texv = subdiv_tex_v[sub_point_cnt - 1];
        intersection[sub_point_cnt][0].color = subdiv_color_v[sub_point_cnt - 1];
        intersection[sub_point_cnt][0].fold = true;
        intersection[sub_point_cnt][0].parallel = false;

        intersection[sub_point_cnt][1].point_count = 1;
        intersection[sub_point_cnt][1].next_segment_id = sub_point_cnt;
        intersection[sub_point_cnt][1].point = xformed_subdiv_pts[sub_point_cnt - 1];
        intersection[sub_point_cnt][1].texv = subdiv_tex_v[sub_point_cnt - 1];
        intersection[sub_point_cnt][1].color = subdiv_color_v[sub_point_cnt - 1];
        intersection[sub_point_cnt][1].fold = true;
        intersection[sub_point_cnt][1].parallel = false;

        Vector3 top = Normalize(xformed_subdiv_pts[sub_point_cnt - 1]
            - Vector3::Dot_Product(segment[sub_point_cnt - 1].edge_plane[0], xformed_subdiv_pts[sub_point_cnt - 1])
                * segment[sub_point_cnt - 1].edge_plane[0]);
        intersection[sub_point_cnt][0].direction = top;

        Vector3 bottom = Normalize(xformed_subdiv_pts[sub_point_cnt - 1]
            - Vector3::Dot_Product(segment[sub_point_cnt - 1].edge_plane[1], xformed_subdiv_pts[sub_point_cnt - 1])
                * segment[sub_point_cnt - 1].edge_plane[1]);
        intersection[sub_point_cnt][1].direction = bottom;

        segdir = Normalize(xformed_subdiv_pts[sub_point_cnt - 1] - xformed_subdiv_pts[sub_point_cnt - 2]);
        start_pl = Normalize(
            Vector3::Cross_Product(intersection[sub_point_cnt][0].direction, intersection[sub_point_cnt][1].direction));

        if (Vector3::Dot_Product(segdir, start_pl) <= 0) {
            start_pl = -start_pl;
        }

        segment[sub_point_cnt].start_plane = start_pl;
        segment[sub_point_cnt].edge_plane[0] = start_pl;
        segment[sub_point_cnt].edge_plane[1] = start_pl;

        for (unsigned int sub_point = 2; sub_point < sub_point_cnt; sub_point++) {
            intersection[sub_point][0].point_count = 1;
            intersection[sub_point][0].next_segment_id = sub_point;
            intersection[sub_point][0].point = xformed_subdiv_pts[sub_point - 1];
            intersection[sub_point][0].texv = subdiv_tex_v[sub_point - 1];
            intersection[sub_point][0].color = subdiv_color_v[sub_point - 1];
            intersection[sub_point][1].point_count = 1;
            intersection[sub_point][1].next_segment_id = sub_point;
            intersection[sub_point][1].point = xformed_subdiv_pts[sub_point - 1];
            intersection[sub_point][1].texv = subdiv_tex_v[sub_point - 1];
            intersection[sub_point][1].color = subdiv_color_v[sub_point - 1];

            float dot_top = Vector3::Dot_Product(segment[sub_point - 1].edge_plane[0], segment[sub_point].edge_plane[0]);
            if (GameMath::Fabs(dot_top) >= .9f) {
                Vector3 average_offset;

                if (dot_top <= 0) {
                    average_offset = Normalize(segment[sub_point - 1].edge_plane[0] - segment[sub_point].edge_plane[0]);
                } else {
                    average_offset = Normalize(segment[sub_point - 1].edge_plane[0] + segment[sub_point].edge_plane[0]);
                }

                intersection[sub_point][0].direction = Normalize(xformed_subdiv_pts[sub_point - 1]
                    - Vector3::Dot_Product(average_offset, xformed_subdiv_pts[sub_point - 1]) * average_offset);
                intersection[sub_point][0].parallel = true;
            } else {
                intersection[sub_point][0].direction = Normalize(
                    Vector3::Cross_Product(segment[sub_point - 1].edge_plane[0], segment[sub_point].edge_plane[0]));

                if (Vector3::Dot_Product(intersection[sub_point][0].direction, xformed_subdiv_pts[sub_point - 1]) < 0) {
                    intersection[sub_point][0].direction = -intersection[sub_point][0].direction;
                }

                intersection[sub_point][0].parallel = false;
            }
            float dot_bottom = Vector3::Dot_Product(segment[sub_point - 1].edge_plane[1], segment[sub_point].edge_plane[1]);
            if (GameMath::Fabs(dot_bottom) >= .9f) {
                Vector3 average_offset;

                if (dot_bottom <= 0) {
                    average_offset = Normalize(segment[sub_point - 1].edge_plane[1] - segment[sub_point].edge_plane[1]);
                } else {
                    average_offset = Normalize(segment[sub_point - 1].edge_plane[1] + segment[sub_point].edge_plane[1]);
                }

                intersection[sub_point][1].direction = Normalize(xformed_subdiv_pts[sub_point - 1]
                    - Vector3::Dot_Product(average_offset, xformed_subdiv_pts[sub_point - 1]) * average_offset);
                intersection[sub_point][1].parallel = true;
            } else {
                intersection[sub_point][1].direction = Normalize(
                    Vector3::Cross_Product(segment[sub_point - 1].edge_plane[1], segment[sub_point].edge_plane[1]));

                if (Vector3::Dot_Product(intersection[sub_point][1].direction, xformed_subdiv_pts[sub_point - 1]) < 0) {
                    intersection[sub_point][1].direction = -intersection[sub_point][1].direction;
                }

                intersection[sub_point][1].parallel = false;
            }

            start_pl = Normalize(
                Vector3::Cross_Product(intersection[sub_point][0].direction, intersection[sub_point][1].direction));

            if (Vector3::Dot_Product(segment[sub_point].start_plane, start_pl) <= 0) {
                segment[sub_point].start_plane = -start_pl;
            } else {
                segment[sub_point].start_plane = start_pl;
            }
        }

        if (Is_Merge_Intersections()) {
            bool merged = true;

            while (merged) {
                merged = false;

                for (unsigned int edge_type = 0; edge_type < NUM_EDGES; edge_type++) {
                    unsigned int num_isects = num_intersections[edge_type];
                    unsigned int iidx_r = 1;
                    unsigned int iidx_w = 1;

                    while (iidx_r < num_isects) {
                        auto &edge3 = intersection[iidx_r][edge_type];
                        auto &edge2 = intersection[iidx_r + 1][edge_type];
                        auto &edge4 = intersection[iidx_w][edge_type];
                        auto &vertex3 = segment[edge2.next_segment_id];
                        auto &vertex1 = segment[edge3.next_segment_id];
                        auto &vertex2 = segment[intersection[iidx_w - 1][edge_type].next_segment_id];

                        for (;;) {
                            if (edge2.fold || Vector3::Dot_Product(edge3.direction, vertex3.start_plane) <= 0
                                || Vector3::Dot_Product(edge3.direction, vertex3.edge_plane[edge_type]) <= 0) {
                                if (edge3.fold || Vector3::Dot_Product(edge2.direction, -vertex1.start_plane) <= 0
                                    || Vector3::Dot_Product(edge2.direction, vertex2.edge_plane[edge_type]) <= 0) {
                                    break;
                                }
                            }

                            unsigned int new_count = edge2.point_count + edge3.point_count;
                            float curr_factor = 1.f / (float)new_count * (float)edge3.point_count;
                            Vector3 new_point = (curr_factor * edge2.point) + (curr_factor * edge3.point);
                            float new_tex_v = curr_factor * edge3.texv + curr_factor * edge2.texv;
                            float dot_top =
                                Vector3::Dot_Product(vertex2.edge_plane[edge_type], vertex3.edge_plane[edge_type]);
                            Vector3 new_direction;
                            bool new_parallel;

                            if (GameMath::Fabs(dot_top) >= .9f) {
                                Vector3 pl;

                                if (dot_top <= 0) {
                                    pl = Normalize(vertex2.edge_plane[edge_type] - vertex3.edge_plane[edge_type]);
                                } else {
                                    pl = Normalize(vertex2.edge_plane[edge_type] + vertex3.edge_plane[edge_type]);
                                }

                                if (edge3.parallel) {
                                    new_direction =
                                        Normalize(edge3.direction - Vector3::Dot_Product(pl, edge3.direction) * pl);
                                } else {
                                    new_direction = Normalize(Vector3::Cross_Product(vertex1.edge_plane[edge_type], pl));
                                }

                                new_parallel = true;
                            } else {
                                new_direction = Normalize(
                                    Vector3::Cross_Product(vertex2.edge_plane[edge_type], vertex3.edge_plane[edge_type]));

                                if (Vector3::Dot_Product(new_direction, new_point) < 0) {
                                    new_direction = -new_direction;
                                }

                                new_parallel = false;
                            }

                            if (m_mergeAbortFactor > 0) {
                                float threshold = (m_mergeAbortFactor * radius) * (m_mergeAbortFactor * radius);

                                if ((edge3.point - Vector3::Dot_Product(edge3.point, new_direction) * new_direction)
                                            .Length2()
                                        > threshold
                                    || (edge2.point - Vector3::Dot_Product(edge2.point, new_direction) * new_direction)
                                            .Length2()
                                        > threshold) {
                                    break;
                                }
                            }

                            merged = true;
                            edge3.point_count = new_count;
                            edge3.next_segment_id = edge2.next_segment_id;
                            edge3.direction = new_direction;
                            edge3.point = new_point;
                            edge3.texv = new_tex_v;
                            edge3.fold = (edge3.fold || edge2.fold);
                            edge3.parallel = new_parallel;
                            num_intersections[edge_type]--;
                            iidx_r++;

                            if (iidx_r == num_isects) {
                                break;
                            }

                            edge2 = intersection[iidx_r + 1][edge_type];
                            vertex3 = segment[edge2.next_segment_id];
                        }

                        edge4.point_count = edge3.point_count;
                        edge4.next_segment_id = edge3.next_segment_id;
                        edge4.direction = edge3.direction;
                        edge4.point = edge3.point;
                        edge4.texv = edge3.texv;
                        edge4.fold = edge3.fold;
                        iidx_r++;
                        iidx_w++;
                    }

                    if (iidx_r == num_isects) {
                        auto &intersection_w = intersection[iidx_w][edge_type];
                        auto &intersection_r = intersection[iidx_r][edge_type];
                        intersection_w.point_count = intersection_r.point_count;
                        intersection_w.next_segment_id = intersection_r.next_segment_id;
                        intersection_w.direction = intersection_r.direction;
                        intersection_w.point = intersection_r.point;
                        intersection_w.texv = intersection_r.texv;
                        intersection_w.fold = intersection_r.fold;
                    }
                }
            }
        }

        unsigned int vnum = num_intersections[BOTTOM_EDGE] + num_intersections[TOP_EDGE];
        VertexFormatXYZDUV1 *v_vertex_array = Get_Vertex_Buffer(vnum);
        Vector3i16 v_index_array[256];
        unsigned short vertex_count = 0;
        unsigned int tri_count = 0;

        top = Vector3::Dot_Product(xformed_subdiv_pts[0], intersection[1][0].direction) * intersection[1][0].direction;
        v_vertex_array[vertex_count].x = top.X;
        v_vertex_array[vertex_count].y = top.Y;
        v_vertex_array[vertex_count].z = top.Z;
        v_vertex_array[vertex_count].diffuse = DX8Wrapper::Convert_Color(intersection[1][0].color);
        v_vertex_array[vertex_count].u1 = u_values[0] + uv_offset.X;
        v_vertex_array[vertex_count].v1 = intersection[1][0].texv + uv_offset.Y;
        vertex_count++;

        bottom = Vector3::Dot_Product(xformed_subdiv_pts[0], intersection[1][1].direction) * intersection[1][1].direction;
        v_vertex_array[vertex_count].x = bottom.X;
        v_vertex_array[vertex_count].y = bottom.Y;
        v_vertex_array[vertex_count].z = bottom.Z;
        v_vertex_array[vertex_count].diffuse = DX8Wrapper::Convert_Color(intersection[1][1].color);
        v_vertex_array[vertex_count].u1 = u_values[1] + uv_offset.X;
        v_vertex_array[vertex_count].v1 = intersection[1][1].texv + uv_offset.Y;
        vertex_count++;

        unsigned short last_top_vidx = 0;
        unsigned short last_bottom_vidx = 1;
        unsigned int top_int_idx = 1;
        unsigned int bottom_int_idx = 1;
        unsigned int pidx = std::min(intersection[1][0].point_count, intersection[1][1].point_count) - 1;
        unsigned int residual_top_points = intersection[1][0].point_count - pidx;
        unsigned int residual_bottom_points = intersection[1][1].point_count - pidx;
        unsigned int i = pidx;

        do {
            if (residual_top_points == 1 && residual_bottom_points == 1) {
                v_index_array[tri_count].I = last_top_vidx;
                v_index_array[tri_count].J = last_bottom_vidx;
                v_index_array[tri_count].K = vertex_count;
                tri_count++;

                v_index_array[tri_count].I = last_bottom_vidx;
                v_index_array[tri_count].J = vertex_count + 1;
                v_index_array[tri_count].K = vertex_count;
                tri_count++;

                last_top_vidx = vertex_count;
                last_bottom_vidx = vertex_count + 1;
                top_int_idx++;
                bottom_int_idx++;
                residual_top_points = intersection[top_int_idx][0].point_count;
                residual_bottom_points = intersection[bottom_int_idx][1].point_count;
                i++;

                top = Vector3::Dot_Product(xformed_subdiv_pts[i], intersection[top_int_idx][0].direction)
                    * intersection[top_int_idx][0].direction;
                bottom = Vector3::Dot_Product(xformed_subdiv_pts[i], intersection[bottom_int_idx][1].direction)
                    * intersection[bottom_int_idx][1].direction;

                v_vertex_array[vertex_count].x = top.X;
                v_vertex_array[vertex_count].y = top.Y;
                v_vertex_array[vertex_count].z = top.Z;
                v_vertex_array[vertex_count].diffuse = DX8Wrapper::Convert_Color(intersection[top_int_idx][0].color);
                v_vertex_array[vertex_count].u1 = u_values[0] + uv_offset.X;
                v_vertex_array[vertex_count].v1 = intersection[top_int_idx][0].texv + uv_offset.Y;
                vertex_count++;

                v_vertex_array[vertex_count].x = bottom.X;
                v_vertex_array[vertex_count].y = bottom.Y;
                v_vertex_array[vertex_count].z = bottom.Z;
                v_vertex_array[vertex_count].diffuse = DX8Wrapper::Convert_Color(intersection[bottom_int_idx][1].color);
                v_vertex_array[vertex_count].u1 = u_values[1] + uv_offset.X;
                v_vertex_array[vertex_count].v1 = intersection[bottom_int_idx][1].texv + uv_offset.Y;
                vertex_count++;
            } else if (residual_top_points > 1) {
                v_index_array[tri_count].I = last_top_vidx;
                v_index_array[tri_count].J = last_bottom_vidx;
                v_index_array[tri_count].K = vertex_count;
                tri_count++;

                last_bottom_vidx = vertex_count;
                residual_top_points--;
                bottom_int_idx++;
                residual_bottom_points = intersection[bottom_int_idx][1].point_count;
                i++;

                bottom = Vector3::Dot_Product(xformed_subdiv_pts[i], intersection[bottom_int_idx][1].direction)
                    * intersection[bottom_int_idx][1].direction;
                v_vertex_array[vertex_count].x = bottom.X;
                v_vertex_array[vertex_count].y = bottom.Y;
                v_vertex_array[vertex_count].z = bottom.Z;
                v_vertex_array[vertex_count].diffuse = DX8Wrapper::Convert_Color(intersection[bottom_int_idx][1].color);
                v_vertex_array[vertex_count].u1 = u_values[1] + uv_offset.X;
                v_vertex_array[vertex_count].v1 = intersection[bottom_int_idx][1].texv + uv_offset.Y;
                vertex_count++;
            } else {
                v_index_array[tri_count].I = last_top_vidx;
                v_index_array[tri_count].J = last_bottom_vidx;
                v_index_array[tri_count].K = vertex_count;
                tri_count++;

                last_top_vidx = vertex_count;
                residual_bottom_points--;
                top_int_idx++;
                residual_top_points = intersection[top_int_idx][0].point_count;
                i++;

                top = Vector3::Dot_Product(xformed_subdiv_pts[i], intersection[top_int_idx][0].direction)
                    * intersection[top_int_idx][0].direction;
                v_vertex_array[vertex_count].x = top.X;
                v_vertex_array[vertex_count].y = top.Y;
                v_vertex_array[vertex_count].z = top.Z;
                v_vertex_array[vertex_count].diffuse = DX8Wrapper::Convert_Color(intersection[top_int_idx][0].color);
                v_vertex_array[vertex_count].u1 = u_values[0] + uv_offset.X;
                v_vertex_array[vertex_count].v1 = intersection[top_int_idx][0].texv + uv_offset.Y;
                vertex_count++;
            }

            int residual_points;

            if (residual_top_points >= residual_bottom_points) {
                residual_points = residual_bottom_points;
            } else {
                residual_points = residual_top_points;
            }

            residual_top_points -= residual_points - 1;
            residual_bottom_points -= residual_points - 1;
            i += residual_points - 1;
        } while ((top_int_idx < num_intersections[TOP_EDGE] || residual_top_points != 1)
            && (bottom_int_idx < num_intersections[BOTTOM_EDGE] || residual_bottom_points != 1));

        unsigned int color = DX8Wrapper::Convert_Color(m_color, m_opacity);
        bool b = color == 0xFFFFFFFF;
        bool sorting = !Is_Sorting_Disabled() && m_shader.Get_Dst_Blend_Func() != ShaderClass::DSTBLEND_ZERO
            && m_shader.Get_Alpha_Test() == ShaderClass::ALPHATEST_DISABLE;

        ShaderClass shader = m_shader;
        shader.Set_Cull_Mode(ShaderClass::CULL_MODE_DISABLE);
        VertexMaterialClass *material = nullptr;

        if (b && !color) {
            shader.Set_Primary_Gradient(ShaderClass::GRADIENT_DISABLE);
            material = VertexMaterialClass::Get_Preset(VertexMaterialClass::PRELIT_NODIFFUSE);
        } else {
            shader.Set_Primary_Gradient(ShaderClass::GRADIENT_MODULATE);
            material = VertexMaterialClass::Get_Preset(VertexMaterialClass::PRELIT_DIFFUSE);
        }

        if (m_texture) {
            shader.Set_Texturing(ShaderClass::TEXTURING_ENABLE);
        } else {
            shader.Set_Texturing(ShaderClass::TEXTURING_DISABLE);
        }

        DynamicVBAccessClass vb(sorting + 2, DX8_FVF_XYZNDUV2, vnum);
        {
            DynamicVBAccessClass::WriteLockClass lock(&vb);
            VertexFormatXYZNDUV2 *vertex_array = lock.Get_Formatted_Vertex_Array();

            for (unsigned int vertex = 0; vertex < vnum; vertex++) {
                vertex_array[vertex].x = v_vertex_array[vertex].x;
                vertex_array[vertex].y = v_vertex_array[vertex].y;
                vertex_array[vertex].z = v_vertex_array[vertex].z;
                vertex_array[vertex].nx = 0;
                vertex_array[vertex].ny = 0;
                vertex_array[vertex].nz = 0;
                vertex_array[vertex].diffuse = v_vertex_array[vertex].diffuse;
                vertex_array[vertex].u1 = v_vertex_array[vertex].u1;
                vertex_array[vertex].v1 = v_vertex_array[vertex].v1;
                vertex_array[vertex].u2 = 0;
                vertex_array[vertex].v2 = 0;
            }
        }

        DynamicIBAccessClass ib(sorting + 2, 3 * tri_count);
        {
            DynamicIBAccessClass::WriteLockClass lock(&ib);
            unsigned short *index_array = lock.Get_Index_Array();
            memcpy(index_array, v_index_array, tri_count * sizeof(Vector3i16));
        }

        DX8Wrapper::Set_Index_Buffer(ib, 0);
        DX8Wrapper::Set_Vertex_Buffer(vb);
        DX8Wrapper::Set_Material(material);
        DX8Wrapper::Set_Texture(0, m_texture);
        DX8Wrapper::Set_Shader(shader);

        if (sorting) {
            SortingRendererClass::Insert_Triangles(obj_sphere, 0, tri_count, 0, vnum);
        } else {
            DX8Wrapper::Draw_Triangles(0, tri_count, 0, vnum);
        }

        Ref_Ptr_Release(material);
    }

    DX8Wrapper::Set_Transform(D3DTS_VIEW, view);
#endif
}

void SegLineRendererClass::Scale(float scale)
{
    m_width *= scale;
    m_noiseAmplitude *= scale;
}

void SegLineRendererClass::Subdivision_Util(unsigned int point_cnt,
    const Vector3 *xformed_pts,
    const float *base_tex_v,
    unsigned int *p_sub_point_cnt,
    Vector3 *xformed_subdiv_pts,
    float *subdiv_tex_v,
    Vector4 *base_color_v,
    Vector4 *subdiv_color_v)
{
    struct SegLineSubdivision
    {
        Vector3 start_pos;
        Vector3 end_pos;
        float start_texv;
        float end_texv;
        Vector4 start_color;
        Vector4 end_color;
        float rand;
        unsigned int level;
    };

    int freeze_random = Is_Freeze_Random();
    Random3Class randomize;
    Vector3SolidBoxRandomizer randomizer(Vector3(1, 1, 1));
    Vector3 randvec(0, 0, 0);
    SegLineSubdivision stack[MAX_SEGLINE_SUBDIV_LEVELS * 2];
    unsigned int output_index = 0;

    for (unsigned int input_index = 0; input_index < point_cnt - 1; input_index++) {
        stack[0].start_pos = xformed_pts[input_index];
        stack[0].end_pos = xformed_pts[input_index + 1];
        stack[0].start_texv = base_tex_v[input_index];
        stack[0].end_texv = base_tex_v[input_index + 1];

        if (base_color_v) {
            stack[0].start_color = base_color_v[input_index];
            stack[0].end_color = base_color_v[input_index + 1];
        } else {
            stack[0].start_color.Set(m_color.X, m_color.Y, m_color.Z, m_opacity);
            stack[0].end_color = stack[0].start_color;
        }

        stack[0].rand = m_noiseAmplitude;
        stack[0].level = 0;

        for (int i = 0; i >= 0;) {
            if (stack[i].level == m_subdivisionLevel) {
                xformed_subdiv_pts[output_index] = stack[i].start_pos;
                subdiv_tex_v[output_index] = stack[i].start_texv;
                subdiv_color_v[output_index] = stack[i].start_color;
                output_index++;
                i--;
            } else {
                if (freeze_random) {
                    randvec.Set(4.6566129e-10f * randomize(), 4.6566129e-10f * randomize(), 4.6566129e-10f * randomize());
                } else {
                    randomizer.Get_Vector(randvec);
                }

                stack[i + 1].start_pos = stack[i].start_pos;
                stack[i + 1].end_pos = (stack[i].start_pos + stack[i].end_pos) * .5f + stack[i].rand * randvec;
                stack[i + 1].start_texv = stack[i].start_texv;
                stack[i + 1].end_texv = (stack[i].start_texv + stack[i].end_texv) * .5f;
                stack[i + 1].start_color = stack[i].start_color;
                stack[i + 1].end_color = (stack[i].start_color + stack[i].end_color) * .5f;
                stack[i + 1].rand = stack[i].rand * .5f;
                stack[i + 1].level = stack[i].level + 1;
                stack[i].start_pos = stack[i + 1].end_pos;
                stack[i].start_texv = stack[i + 1].end_texv;
                stack[i].rand = stack[i + 1].rand;
                stack[i].level = stack[i + 1].level;
                i++;
            }
        }
    }
    xformed_subdiv_pts[output_index] = xformed_pts[point_cnt - 1];
    subdiv_tex_v[output_index] = base_tex_v[point_cnt - 1];

    if (base_color_v) {
        subdiv_color_v[output_index] = base_color_v[point_cnt - 1];
    } else {
        subdiv_color_v[output_index].Set(m_color.X, m_color.Y, m_color.Z, m_opacity);
    }

    *p_sub_point_cnt = output_index + 1;
}

VertexFormatXYZDUV1 *SegLineRendererClass::Get_Vertex_Buffer(int count)
{
    if (count > m_vertexCount) {
        delete[] m_vertexBuffer;
        int c = count + (count >> 1);
        m_vertexCount = c;
        m_vertexBuffer = new VertexFormatXYZDUV1[c];
    }
    return m_vertexBuffer;
}
