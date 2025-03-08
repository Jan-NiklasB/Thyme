# Build and link the DLL.
set(GAMEENGINE_INCLUDES
    editor
    game
    game/client
    game/client/draw
    game/client/gui
    game/client/input
    game/client/messagestream
    game/client/shadow
    game/client/shader
    game/client/system
    game/client/system/particlesystem
    game/client/videoplayer
    game/client/water
    game/common
    game/common/audio
    game/common/compression
    game/common/ini
    game/common/modules
    game/common/rts
    game/common/system
    game/common/system/savegame
    game/common/thing
    game/logic/ai
    game/logic/map
    game/logic/object
    game/logic/object/behavior
    game/logic/object/body
    game/logic/object/contain
    game/logic/object/die
    game/logic/object/update
    game/logic/scriptengine
    game/logic/system
    game/network
    platform
    platform/audio/
    platform/directx/
    platform/input/
    platform/video/
    platform/w3dengine/client
    platform/w3dengine/common/thing
    w3d/lib
    w3d/math
    w3d/renderer
    w3d/saveload
)

set(HOOKER_SRC
    hooker/dllmain.cpp
    hooker/mapview.cpp
    hooker/setupglobals_zh.cpp
)

set(HOOKER_WB_SRC
    hooker/setuphooks_wb.cpp
)

set(HOOKER_GAME_SRC
    hooker/setuphooks_zh.cpp
)

set(GAMEENGINE_SRC
    game/copyprotect.cpp
    game/crashwrapper.cpp
    game/force_nocd.cpp
    game/maputil.cpp
    game/weather.cpp
    game/client/baseheightmap.cpp
    game/client/line2d.cpp
    game/client/camerashakesystem.cpp
    game/client/challengegenerals.cpp
    game/client/crashpreferences.cpp
    game/client/display.cpp
    game/client/displaystring.cpp
    game/client/displaystringmanager.cpp
    game/client/drawgroupinfo.cpp
    game/client/flatheightmap.cpp
    game/client/fxlist.cpp
    game/client/gameclient.cpp
    game/client/gametext.cpp
    game/client/globallanguage.cpp
    game/client/heightmap.cpp
    game/client/ingameui.cpp
    game/client/languagefilter.cpp
    game/client/optionpreferences.cpp
    game/client/parabolicease.cpp
    game/client/shadermanager.cpp
    game/client/terrainroads.cpp
    game/client/terraintex.cpp
    game/client/terrainvisual.cpp
    game/client/tiledata.cpp
    game/client/view.cpp
    game/client/water.cpp
    game/client/worldheightmap.cpp
    game/client/draw/drawable.cpp
    game/client/draw/w3ddebrisdraw.cpp
    game/client/draw/w3ddefaultdraw.cpp
    game/client/draw/w3ddependencymodeldraw.cpp
    game/client/draw/w3dlaserdraw.cpp
    game/client/draw/w3dmodeldraw.cpp
    game/client/draw/w3doverlordaircraftdraw.cpp
    game/client/draw/w3doverlordtankdraw.cpp
    game/client/draw/w3doverlordtruckdraw.cpp
    game/client/draw/w3dpolicecardraw.cpp
    game/client/draw/w3dprojectilestreamdraw.cpp
    game/client/draw/w3dpropdraw.cpp
    game/client/draw/w3dropedraw.cpp
    game/client/draw/w3dsciencemodeldraw.cpp
    game/client/draw/w3dsupplydraw.cpp
    game/client/draw/w3dtankdraw.cpp
    game/client/draw/w3dtanktruckdraw.cpp
    game/client/draw/w3dtracerdraw.cpp
    game/client/draw/w3dtreedraw.cpp
    game/client/draw/w3dtruckdraw.cpp
    game/client/draw/tintenvelope.cpp
    game/client/gui/gamefont.cpp
    game/client/gui/gamewindow.cpp
    game/client/gui/gamewindowmanager.cpp
    game/client/gui/gamewindowtransitions.cpp
    game/client/gui/imemanagerinterface.cpp
    game/client/gui/imemanager.cpp
    game/client/gui/loadscreen.cpp
    game/client/gui/wininstancedata.cpp
    game/client/gui/windowlayout.cpp
    game/client/gui/w3dgamefont.cpp
    game/client/image.cpp
    game/client/anim2d.cpp
    game/client/input/keyboard.cpp
    game/client/input/mouse.cpp
    game/client/messagestream/metaevent.cpp
    game/client/shadow/w3dbuffermanager.cpp
    game/client/shadow/w3dprojectedshadow.cpp
    game/client/shadow/w3dvolumetricshadow.cpp
    game/client/shadow/w3dshadow.cpp
    game/client/shader/bwfilter.cpp
    game/client/shader/cloudshader.cpp
    game/client/shader/crossfadefilter.cpp
    game/client/shader/defaultfilter.cpp
    game/client/shader/flatshroudshader.cpp
    game/client/shader/flatterrainshader.cpp
    game/client/shader/motionblurfilter.cpp
    game/client/shader/maskshader.cpp
    game/client/shader/roadshader.cpp
    game/client/shader/shroudshader.cpp
    game/client/shader/terrainshader.cpp
    game/client/system/debugdisplay.cpp
    game/client/system/rayeffect.cpp
    game/client/system/particlesystem/particle.cpp
    game/client/system/particlesystem/particleinfo.cpp
    game/client/system/particlesystem/particlesys.cpp
    game/client/system/particlesystem/particlesysinfo.cpp
    game/client/system/particlesystem/particlesysmanager.cpp
    game/client/system/particlesystem/particlesystemplate.cpp
    game/client/system/smudge.cpp
    game/client/videoplayer/videobuffer.cpp
    game/client/videoplayer/videoplayer.cpp
    game/client/videoplayer/videostream.cpp
    game/client/water/w3dwater.cpp
    game/client/water/w3dwatertracks.cpp
    game/common/bezfwditerator.cpp
    game/common/beziersegment.cpp
    game/common/bitflags.cpp
    game/common/commandline.cpp
    game/common/commandlist.cpp
    game/common/crc.cpp
    game/common/damagefx.cpp
    game/common/dict.cpp
    game/common/gameengine.cpp
    game/common/gamelod.cpp
    game/common/gamemain.cpp
    game/common/gamemessage.cpp
    game/common/gamemessagelist.cpp
    game/common/globaldata.cpp
    game/common/namekeygenerator.cpp
    game/common/mapobject.cpp
    game/common/messagestream.cpp
    game/common/multiplayersettings.cpp
    game/common/randomvalue.cpp
    game/common/recorder.cpp
    game/common/registry.cpp
    game/common/staticnamekey.cpp
    game/common/statemachine.cpp
    game/common/terraintypes.cpp
    game/common/userpreferences.cpp
    game/common/version.cpp
    game/common/audio/audioeventinfo.cpp
    game/common/audio/audioeventrts.cpp
    game/common/audio/audiomanager.cpp
    game/common/audio/audiomisc.cpp
    game/common/audio/audiosettings.cpp
    game/common/audio/musicmanager.cpp
    game/common/audio/soundmanager.cpp
    game/common/compression/compressionmanager.cpp
    game/common/compression/refpack.cpp
    game/common/ini/ini.cpp
    game/common/ini/inidrawgroupinfo.cpp
    game/common/modules/behaviormodule.cpp
    game/common/modules/module.cpp
    game/common/modules/modulefactory.cpp
    game/common/modules/updatemodule.cpp
    game/common/rts/academystats.cpp
    game/common/rts/actionmanager.cpp
    game/common/rts/buildinfo.cpp
    game/common/rts/energy.cpp
    game/common/rts/handicap.cpp
    game/common/rts/money.cpp
    game/common/rts/missionstats.cpp
    game/common/rts/player.cpp
    game/common/rts/playerlist.cpp
    game/common/rts/playertemplate.cpp
    game/common/rts/productionprerequisite.cpp
    game/common/rts/science.cpp
    game/common/rts/scorekeeper.cpp
    game/common/rts/sidesinfo.cpp
    game/common/rts/sideslist.cpp
    game/common/rts/specialpower.cpp
    game/common/rts/team.cpp
    game/common/rts/teamsinfo.cpp
    game/common/system/archivefile.cpp
    game/common/system/archivefilesystem.cpp
    game/common/system/asciistring.cpp
    game/common/system/buildassistant.cpp
    game/common/system/cachedfileinputstream.cpp
    game/common/system/cdmanager.cpp
    game/common/system/datachunk.cpp
    game/common/system/datachunktoc.cpp
    game/common/system/disabledtypes.cpp
    game/common/system/file.cpp
    game/common/system/filesystem.cpp
    game/common/system/functionlexicon.cpp
    game/common/system/gamememory.cpp
    game/common/system/gamememoryinit.cpp
    game/common/system/gametype.cpp
    game/common/system/geometry.cpp
    game/common/system/kindof.cpp
    game/common/system/localfile.cpp
    game/common/system/localfilesystem.cpp
    game/common/system/memblob.cpp
    game/common/system/memdynalloc.cpp
    game/common/system/mempool.cpp
    game/common/system/mempoolfact.cpp
    game/common/system/radar.cpp
    game/common/system/ramfile.cpp
    game/common/system/snapshot.cpp
    game/common/system/stackdump.cpp
    game/common/system/streamingarchivefile.cpp
    game/common/system/subsysteminterface.cpp
    game/common/system/unicodestring.cpp
    game/common/system/upgrade.cpp
    game/common/system/xfer.cpp
    game/common/system/xfercrc.cpp
    game/common/system/savegame/gamestate.cpp
    game/common/system/savegame/gamestatemap.cpp
    game/common/thing/moduleinfo.cpp
    game/common/thing/thing.cpp
    game/common/thing/thingfactory.cpp
    game/common/thing/thingtemplate.cpp
    game/logic/ai/ai.cpp
    game/logic/ai/aipathfind.cpp
    game/logic/ai/aistates.cpp
    game/logic/ai/turretai.cpp
    game/logic/map/polygontrigger.cpp
    game/logic/map/terrainlogic.cpp
    game/logic/object/armor.cpp
    game/logic/object/armortemplateset.cpp
    game/logic/object/experiencetracker.cpp
    game/logic/object/locomotor.cpp
    game/logic/object/object.cpp
    game/logic/object/objectcreationlist.cpp
    game/logic/object/objecttypes.cpp
    game/logic/object/weapon.cpp
    game/logic/object/weaponset.cpp
    game/logic/object/weapontemplateset.cpp
    game/logic/object/update/aiupdate.cpp
    game/logic/object/update/assistedtargetingupdate.cpp
    game/logic/object/update/laserupdate.cpp
    game/logic/object/update/physicsupdate.cpp
    game/logic/object/update/projectilestreamupdate.cpp
    game/logic/object/update/swayclientupdate.cpp
    game/logic/object/die/specialpowercompletiondie.cpp
    game/logic/scriptengine/script.cpp
    game/logic/scriptengine/scriptengine.cpp
    game/logic/scriptengine/scriptaction.cpp
    game/logic/scriptengine/scriptcondition.cpp
    game/logic/scriptengine/scriptgroup.cpp
    game/logic/scriptengine/scriptlist.cpp
    game/logic/scriptengine/scriptparam.cpp
    game/logic/scriptengine/sequentialscript.cpp
    game/logic/scriptengine/victoryconditions.cpp
    game/logic/system/damage.cpp
    game/logic/system/gamelogic.cpp
    game/logic/system/rankinfo.cpp
    game/logic/system/cavesystem.cpp
    game/logic/system/cratesystem.cpp
    game/logic/system/partitionmanager.cpp
    game/network/framemetrics.cpp
    game/network/gameinfo.cpp
    game/network/gameresultsthread.cpp
    game/network/filetransfer.cpp
    game/network/gamemessageparser.cpp
    game/network/ipenumeration.cpp
    game/network/lanapi.cpp
    game/network/lanapiinterface.cpp
    game/network/network.cpp
    game/network/transport.cpp
    game/network/udp.cpp
    game/network/networkutil.cpp
    platform/fpusetting.cpp
    platform/input/win32mouse.cpp
    platform/standardfile.cpp
    platform/w3dfilesystem.cpp
    platform/w3dfunctionlexicon.cpp
    platform/win32bigfile.cpp
    platform/win32bigfilesystem.cpp
    platform/win32gameengine.cpp
    platform/win32localfile.cpp
    platform/win32localfilesystem.cpp
    platform/w3dengine/client/w3dbibbuffer.cpp
    platform/w3dengine/client/w3dbridgebuffer.cpp
    platform/w3dengine/client/w3ddebugdisplay.cpp
    platform/w3dengine/client/w3ddebugicons.cpp
    platform/w3dengine/client/w3ddynamiclight.cpp
    platform/w3dengine/client/w3ddisplay.cpp
    platform/w3dengine/client/w3dgameclient.cpp
    platform/w3dengine/client/w3dpoly.cpp
    platform/w3dengine/client/w3dparticlesys.cpp
    platform/w3dengine/client/w3dpropbuffer.cpp
    platform/w3dengine/client/w3droadbuffer.cpp
    platform/w3dengine/client/w3dshroud.cpp
    platform/w3dengine/client/w3dsmudge.cpp
    platform/w3dengine/client/w3dsnow.cpp
    platform/w3dengine/client/w3dstatuscircle.cpp
    platform/w3dengine/client/w3dscene.cpp
    platform/w3dengine/client/w3dtreebuffer.cpp
    platform/w3dengine/client/w3dview.cpp
    platform/w3dengine/client/w3dmouse.cpp
    platform/w3dengine/client/w3dterrainbackground.cpp
    platform/w3dengine/client/w3dterrainlogic.cpp
    platform/w3dengine/client/w3dterraintracks.cpp
    platform/w3dengine/client/w3dwaypointbuffer.cpp
    platform/w3dengine/common/thing/w3dmodulefactory.cpp
    w3d/lib/buff.cpp
    w3d/lib/bufffileclass.cpp
    w3d/lib/chunkio.cpp
    w3d/lib/cpudetect.cpp
    w3d/lib/critsection.cpp
    w3d/lib/ffactory.cpp
    w3d/lib/fileclass.cpp
    w3d/lib/filestraw.cpp
    w3d/lib/gcd_lcm.cpp
    w3d/lib/iniclass.cpp
    w3d/lib/mpu.cpp
    w3d/lib/multilist.cpp
    w3d/lib/mutex.cpp
    w3d/lib/nstrdup.cpp
    w3d/lib/random.cpp
    w3d/lib/rawfileclass.cpp
    w3d/lib/rcfile.cpp
    w3d/lib/readline.cpp
    w3d/lib/refcount.cpp
    w3d/lib/straw.cpp
    w3d/lib/systimer.cpp
    w3d/lib/targa.cpp
    w3d/lib/thread.cpp
    w3d/lib/threadtrack.cpp
    w3d/lib/wwstring.cpp
    w3d/lib/hash.cpp
    w3d/lib/xstraw.cpp
    w3d/math/aabox.cpp
    w3d/math/colmath.cpp
    w3d/math/colmathaabox.cpp
    w3d/math/colmathaabtri.cpp
    w3d/math/colmathaaplane.cpp
    w3d/math/colmathfrustum.cpp
    w3d/math/colmathline.cpp
    w3d/math/colmathobbobb.cpp
    w3d/math/colmathobbtri.cpp
    w3d/math/colmathplane.cpp
    w3d/math/cullsys.cpp
    w3d/math/frustum.cpp
    w3d/math/gamemath.cpp
    w3d/math/lineseg.cpp
    w3d/math/matrix3.cpp
    w3d/math/matrix3d.cpp
    w3d/math/matrix4.cpp
    w3d/math/quaternion.cpp
    w3d/math/tri.cpp
    w3d/math/v3_rnd.cpp
    w3d/math/vector4.cpp
    w3d/math/vp.cpp
    w3d/renderer/animobj.cpp
    w3d/renderer/aabtree.cpp
    w3d/renderer/aabtreebuilder.cpp
    w3d/renderer/assetmgr.cpp
    w3d/renderer/bitmaphandler.cpp
    w3d/renderer/bmp2d.cpp
    w3d/renderer/boxrobj.cpp
    w3d/renderer/bwrender.cpp
    w3d/renderer/camera.cpp
    w3d/renderer/coltest.cpp
    w3d/renderer/colorspace.cpp
    w3d/renderer/composite.cpp
    w3d/renderer/dazzle.cpp
    w3d/renderer/ddsfile.cpp
    w3d/renderer/dx8caps.cpp
    w3d/renderer/dx8fvf.cpp
    w3d/renderer/dx8indexbuffer.cpp
    w3d/renderer/dx8polygonrenderer.cpp
    w3d/renderer/dx8renderer.cpp
    w3d/renderer/dx8texman.cpp
    w3d/renderer/dx8vertexbuffer.cpp
    w3d/renderer/dx8wrapper.cpp
    w3d/renderer/dynamesh.cpp
    w3d/renderer/hanimmgr.cpp
    w3d/renderer/hcanim.cpp
    w3d/renderer/hlod.cpp
    w3d/renderer/hrawanim.cpp
    w3d/renderer/htree.cpp
    w3d/renderer/htreemgr.cpp
    w3d/renderer/intersec.cpp
    w3d/renderer/light.cpp
    w3d/renderer/linegroup.cpp
    w3d/renderer/lightenv.cpp
    w3d/renderer/line3d.cpp
    w3d/renderer/loaders.cpp
    w3d/renderer/matinfo.cpp
    w3d/renderer/mapper.cpp
    w3d/renderer/matpass.cpp
    w3d/renderer/matrixmapper.cpp
    w3d/renderer/mesh.cpp
    w3d/renderer/meshgeometry.cpp
    w3d/renderer/meshmatdesc.cpp
    w3d/renderer/meshmdl.cpp
    w3d/renderer/missing.cpp
    w3d/renderer/motchan.cpp
    w3d/renderer/nullrobj.cpp
    w3d/renderer/part_buf.cpp
    w3d/renderer/part_emt.cpp
    w3d/renderer/part_ldr.cpp
    w3d/renderer/pivot.cpp
    w3d/renderer/pointgr.cpp
    w3d/renderer/pot.cpp
    w3d/renderer/projector.cpp
    w3d/renderer/render2d.cpp
    w3d/renderer/render2dsentence.cpp
    w3d/renderer/rendobj.cpp
    w3d/renderer/rinfo.cpp
    w3d/renderer/scene.cpp
    w3d/renderer/segline.cpp
    w3d/renderer/seglinerenderer.cpp
    w3d/renderer/shader.cpp
    w3d/renderer/sortingrenderer.cpp
    w3d/renderer/streak.cpp
    w3d/renderer/streakrender.cpp
    w3d/renderer/surfaceclass.cpp
    w3d/renderer/synctextureloadtasklist.cpp
    w3d/renderer/texproject.cpp
    w3d/renderer/texture.cpp
    w3d/renderer/texturebase.cpp
    w3d/renderer/textureloader.cpp
    w3d/renderer/textureloadtask.cpp
    w3d/renderer/textureloadtasklist.cpp
    w3d/renderer/thumbnail.cpp
    w3d/renderer/thumbnailmanager.cpp
    w3d/renderer/vertmaterial.cpp
    w3d/renderer/w3d.cpp
    w3d/renderer/w3d_util.cpp
    w3d/renderer/w3dexclusionlist.cpp
    w3d/renderer/w3dformat.cpp
)

if(USE_STDFS)
    list(APPEND GAMEENGINE_SRC
        platform/stdlocalfilesystem.cpp
    )

    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_STDFS)
endif()

if(USE_ZLIB)
    list(APPEND GAMEENGINE_SRC
        game/common/compression/zlibcompr.cpp
    )
    list(APPEND GAME_LINK_LIBRARIES ZLIB::ZLIB)
    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_ZLIB)
endif()

# Game binary only source
set(GAMEENGINE_GAME_SRC
    game/main.cpp
)

set(GAMEENGINE_WB_SRC
    editor/editmain.cpp
)

# Don't try and build the miles sound system when not building to link to the original game.
if(WIN32 OR "${CMAKE_SYSTEM}" MATCHES "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND NOT OPENAL_FOUND)
        list(APPEND GAMEENGINE_SRC 
        platform/audio/milesaudiofilecache.cpp
        platform/audio/milesaudiomanager.cpp
        platform/video/binkvideoplayer.cpp
        platform/video/binkvideostream.cpp)
    endif()
endif()

if(USE_FFMPEG)
    list(APPEND GAMEENGINE_SRC 
        platform/audio/ffmpegaudiofilecache.cpp)
    list(APPEND GAMEENGINE_INCLUDES ${FFMPEG_INCLUDE_DIRS})
    list(APPEND GAME_LINK_LIBRARIES ${FFMPEG_LIBRARIES})
    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_FFMPEG ${FFMPEG_DEFINITIONS})
endif()

if(DINPUT8_FOUND)
    list(APPEND GAMEENGINE_GAME_SRC
        platform/directx/dinputkeybd.cpp
    )
endif()

if(CMAKE_CONFIGURATION_TYPES)
    # Glob all the header files together to add to the project for msvc/xcode.
    # Not ideal as CMake won't notice if you add any until something else prompts a CMake run
    # but this is largely a convenience feature for MSVC/XCode users.
    file(GLOB_RECURSE GAMEENGINE_HEADERS "*.h")
    set(_GAMEENGINE_SOURCES ${GAMEENGINE_SRC} ${GAMEENGINE_GAME_SRC} ${GAMEENGINE_WB_SRC})

    if(NOT STANDALONE)
        list(APPEND _GAMEENGINE_SOURCES ${HOOKER_SRC} ${HOOKER_GAME_SRC} ${HOOKER_WB_SRC})
    endif()

    # Filter out headers from included 3rd party libraries like STLPort or gamemath.
    list(FILTER GAMEENGINE_HEADERS EXCLUDE REGEX "libs/.*")
    list(FILTER GAMEENGINE_HEADERS EXCLUDE REGEX "base/.*")

    foreach(_GAMEENGINE_SOURCE IN ITEMS ${_GAMEENGINE_SOURCES})
        get_filename_component(_GAMEENGINE_SOURCE_PATH "${_GAMEENGINE_SOURCE}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" _GAMEENGINE_GROUP_PATH "${_GAMEENGINE_SOURCE_PATH}")
        string(REPLACE "/" "\\" _GAMEENGINE_GROUP_PATH "${_GAMEENGINE_GROUP_PATH}")
        source_group("Source Files\\${_GAMEENGINE_GROUP_PATH}" FILES "${_GAMEENGINE_SOURCE}")
    endforeach()

    foreach(_GAMEENGINE_HEADER IN ITEMS ${GAMEENGINE_HEADERS})
        get_filename_component(_GAMEENGINE_HEADER_PATH "${_GAMEENGINE_HEADER}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" _GAMEENGINE_HGROUP_PATH "${_GAMEENGINE_HEADER_PATH}")
        string(REPLACE "/" "\\" _GAMEENGINE_HGROUP_PATH "${_GAMEENGINE_HGROUP_PATH}")
        source_group("Header Files\\${_GAMEENGINE_HGROUP_PATH}" FILES "${_GAMEENGINE_HEADER}")
    endforeach()
endif()

# Gather needed link libraries and compile defintions
list(APPEND GAME_LINK_LIBRARIES base captnlog)

if(USE_SDL2)
    if(TARGET SDL2::SDL2)
        list(APPEND GAME_LINK_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
    else()
        list(APPEND GAME_LINK_LIBRARIES ${SDL2_LIBRARIES})
        list(APPEND GAMEENGINE_INCLUDES ${SDL2_INCLUDE_DIRS})
    endif()
    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_SDL2)

    list(APPEND GAMEENGINE_SRC
        platform/sdl2/sdl2keybd.cpp
        platform/sdl2/sdl2mouse.cpp
        platform/sdl2/sdl2gameengine.cpp
    )

    list(APPEND GAMEENGINE_INCLUDES
        platform/sdl2/
    )
endif()

if(USE_GAMEMATH)
    list(APPEND GAME_LINK_LIBRARIES gamemath_static_lib)
    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_GAMEMATH)
endif()

if(BUILD_DLL)
    # Duplicate that will never link to crashpad for dll injection purposes.
    add_library(crash_handler STATIC game/crashhandler.cpp)
    target_include_directories(crash_handler PRIVATE hooker ${GAMEENGINE_INCLUDES})
    target_link_libraries(crash_handler PRIVATE base captnlog)

    if(ICU_FOUND)
        target_link_libraries(crash_handler PRIVATE ICU::data ICU::i18n ICU::io ICU::tu ICU::uc)
        target_compile_options(crash_handler PRIVATE -DBUILD_WITH_ICU)
    endif()
endif()

if(WIN32 OR "${CMAKE_SYSTEM}" MATCHES "Windows")
    list(APPEND GAME_LINK_LIBRARIES winmm)

    if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND NOT OPENAL_FOUND)
        list(APPEND GAME_LINK_LIBRARIES binkstub milesstub)
        list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_BINK -DBUILD_WITH_MILES)
    endif()
endif()

if(ICU_FOUND)
    list(APPEND GAME_LINK_LIBRARIES ICU::data ICU::i18n ICU::io ICU::tu ICU::uc)
    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_ICU)
endif()

if(D3D8_FOUND)
    list(APPEND GAME_LINK_LIBRARIES d3d8)
endif()

if(DINPUT8_FOUND)
    list(APPEND GAME_LINK_LIBRARIES dxguid dinput8)
    list(APPEND GAME_COMPILE_OPTIONS -DBUILD_WITH_DINPUT)
endif()

# Static library for standalone builds and linking tools.
if(STANDALONE OR BUILD_TOOLS)
    add_library(thyme_lib STATIC ${GAMEENGINE_SRC} ${GAMEENGINE_HEADERS} game/crashhandler.cpp)
    target_include_directories(thyme_lib PUBLIC ${GAMEENGINE_INCLUDES})
    target_link_libraries(thyme_lib PUBLIC ${GAME_LINK_LIBRARIES})
    target_compile_definitions(thyme_lib PUBLIC ${GAME_COMPILE_OPTIONS} "ALLOW_HOOKING=")
    target_compile_definitions(thyme_lib PUBLIC $<$<CONFIG:DEBUG>:GAME_DEBUG> $<$<CONFIG:DEBUG>:GAME_DEBUG_STRUCTS>)

    if(USE_CRASHPAD)
        # Rename the crash handler so it won't conflict with any other installs of crashpad.
        set_target_properties(crashpad_handler PROPERTIES OUTPUT_NAME thymecrashhandler)
        target_compile_options(thyme_lib PUBLIC -DBUILD_WITH_CRASHPAD)
        target_link_libraries(thyme_lib PUBLIC crashpad_client)

        # Force the crashpad handler to be built if we are using crashpad.
        add_dependencies(thyme_lib crashpad_handler)
    endif()
endif()

if(STANDALONE)
    add_executable(thyme ${GAMEENGINE_GAME_SRC})
    target_link_libraries(thyme PRIVATE thyme_lib)
endif()

if(BUILD_DLL)
    include(TargetExports)
    add_library(thyme_dll SHARED ${HOOKER_SRC} ${HOOKER_GAME_SRC} ${GAMEENGINE_GAME_SRC} ${GAMEENGINE_SRC} ${GAMEENGINE_HEADERS})
    target_include_directories(thyme_dll BEFORE PRIVATE ${CMAKE_SOURCE_DIR}/deps/stlport)
    target_include_directories(thyme_dll PRIVATE hooker ${GAMEENGINE_INCLUDES})
    target_compile_definitions(thyme_dll PRIVATE -DGAME_DLL -DTHYME_USE_STLPORT -D_USE_32BIT_TIME_T)
    target_link_libraries(thyme_dll ${GAME_LINK_LIBRARIES} crash_handler)
    target_compile_definitions(thyme_dll PRIVATE ${GAME_COMPILE_OPTIONS} "ALLOW_HOOKING=friend void Setup_Hooks()\;")
    target_compile_definitions(thyme_dll PRIVATE $<$<CONFIG:DEBUG>:GAME_DEBUG>)
    set_target_properties(thyme_dll PROPERTIES OUTPUT_NAME thyme PDB_NAME thymedll)
    target_exports(thyme_dll SYMBOLS 
        Setup_Hooks
    )
endif()

# Note that we don't care about which math lib we link against for worldbuilder.
# Since worldbuilder isn't multiplayer we don't need to worry about sources of indeterminancy
if(BUILD_EDITOR)
    set(CMAKE_MFC_FLAG 2) #Enable MFC
    if(STANDALONE)
        add_executable(thymeedit ${GAMEENGINE_WB_SRC})
        target_link_libraries(thymeedit PRIVATE thyme_lib)
    endif()

    if(BUILD_DLL)
        add_library(thymeedit_dll SHARED ${HOOKER_SRC} ${HOOKER_WB_SRC} ${GAMEENGINE_WB_SRC} ${GAMEENGINE_SRC} ${GAMEENGINE_HEADERS})
        target_include_directories(thymeedit_dll BEFORE PRIVATE ${CMAKE_SOURCE_DIR}/deps/stlport)
        target_include_directories(thymeedit_dll PRIVATE hooker ${GAMEENGINE_INCLUDES})
        target_compile_definitions(thymeedit_dll PRIVATE -DGAME_DLL -DTHYME_USE_STLPORT -DGAME_DEBUG_STRUCTS -DBUILD_EDITOR -D_USE_32BIT_TIME_T)
        target_link_libraries(thymeedit_dll ${GAME_LINK_LIBRARIES} crash_handler)
        target_compile_definitions(thymeedit_dll PRIVATE ${GAME_COMPILE_OPTIONS} "ALLOW_HOOKING=friend void Setup_Hooks()\;")
        target_compile_definitions(thymeedit_dll PRIVATE $<$<CONFIG:DEBUG>:GAME_DEBUG>)
        set_target_properties(thymeedit_dll PROPERTIES OUTPUT_NAME thymeedit PDB_NAME thymeeditdll)
        target_exports(thymeedit_dll SYMBOLS 
            Setup_Hooks
        )
    endif()
endif()

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()
