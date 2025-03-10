/**
 * @file
 *
 * @author Jonathan Wilson
 *
 * @brief W3d Snow Manager
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "always.h"

class W3DSnowManager
{
public:
    void Release_Resources();
    void Re_Acquire_Resources();
};

#ifdef GAME_DLL
extern W3DSnowManager *&g_theSnowManager;
#else
extern W3DSnowManager *g_theSnowManager;
#endif
