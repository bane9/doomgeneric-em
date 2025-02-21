// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

#include "i_video.h"
#include "z_zone.h"

#include "doomgeneric.h"

#define NCOLORS 256

static color colors[NCOLORS];
byte *I_VideoBuffer;

void I_InitGraphics(void)
{
    I_VideoBuffer =
        (byte *) Z_Malloc(SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);
}

void I_FinishUpdate(void)
{
    const int x_ratio = (SCREENWIDTH << 16) / (DOOMGENERIC_RESX);
    const int y_ratio = (SCREENHEIGHT << 16) / (DOOMGENERIC_RESY);

    for (int y = 0; y < (DOOMGENERIC_RESY); y++)
    {
        const int y2_xsource = ((y * y_ratio) >> 16) * SCREENWIDTH;
        const int i_xdest = y * (DOOMGENERIC_RESX);

        for (int x = 0; x < (DOOMGENERIC_RESX); x++)
        {
            const int x2 = ((x * x_ratio) >> 16);
            const int y2_x2_colors = y2_xsource + x2;
            const int i_x_colors = i_xdest + x;

            DG_ScreenBuffer[i_x_colors] = colors[I_VideoBuffer[y2_x2_colors]];
        }
    }

    DG_DrawFrame();
}

// clang-format off
#define GFX_RGB565(r, g, b)	(((((r) & 0xF8) >> 3) << 11) | \
                             ((((g) & 0xFC) >> 2) << 5)  | \
                             (((b) & 0xF8) >> 3))

#define GFX_ARGB(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define GFX_RGBA(r, g, b, a) (((r) << 24) | ((g) << 16) | ((b) << 8) | (a))
#define GFX_ABGR(r, g, b, a) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
#define GFX_BGRA(r, g, b, a) (((b) << 24) | ((g) << 16) | ((r) << 8) | (a))
// clang-format on

void I_SetPalette(byte *palette)
{
    for (int i = 0; i < NCOLORS; i++)
    {
        uint8_t r = *palette++;
        uint8_t g = *palette++;
        uint8_t b = *palette++;

#ifdef DOOMGENERIC_FB_565
        colors[i] = GFX_RGB565(r, g, b);
#else
        colors[i] = GFX_ARGB(r, g, b, 0);
#endif
    }
}

void I_StartTic(void)
{
    I_GetEvent();
}

void I_ReadScreen(byte *scr)
{
    memcpy(scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

void I_SetWindowTitle(char *title)
{
    DG_SetWindowTitle(title);
}
