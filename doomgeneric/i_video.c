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
#include "config.h"
#include "d_event.h"
#include "d_main.h"
#include "m_argv.h"
#include "v_video.h"
#include "z_zone.h"

#include "doomkeys.h"
#include "tables.h"

#include "doomgeneric.h"

#include <stdbool.h>
#include <stdlib.h>

#include <fcntl.h>

#include <stdarg.h>

#include <sys/types.h>

#include "doomgeneric_syscall.h"

int usemouse = 0;

static color colors[256];

void I_GetEvent(void);

// The screen buffer; this is modified to draw things to the screen

byte *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

boolean screensaver_mode = false;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen

boolean screenvisible;

// Mouse acceleration
//
// This emulates some of the behavior of DOS mouse drivers by increasing
// the speed when the mouse is moved fast.
//
// The mouse input values are input directly to the game, but when
// the values exceed the value of mouse_threshold, they are multiplied
// by mouse_acceleration to increase the speed.

float mouse_acceleration = 2.0;
int mouse_threshold = 10;

// Gamma correction level to use

int usegamma = 0;

typedef struct
{
    byte r;
    byte g;
    byte b;
} col_t;

void I_InitGraphics(void)
{
    /* Allocate screen to draw to */
    I_VideoBuffer = (byte *) Z_Malloc(SCREENWIDTH * SCREENHEIGHT, PU_STATIC,
                                      NULL); // For DOOM to draw on

    screenvisible = true;

    extern void I_InitInput(void);
    I_InitInput();
}
void I_StartFrame(void)
{
}

void I_StartTic(void)
{
    I_GetEvent();
}

void I_UpdateNoBlit(void)
{
}

//
// I_FinishUpdate
//

void I_FinishUpdate(void)
{
    const int x_ratio = (SCREENWIDTH << 16) / DOOMGENERIC_RESX;
    const int y_ratio = (SCREENHEIGHT << 16) / DOOMGENERIC_RESY;

    for (int y = 0; y < DOOMGENERIC_RESY; y++)
    {
        const int y2_xsource = ((y * y_ratio) >> 16) * SCREENWIDTH;
        const int i_xdest = y * DOOMGENERIC_RESX;

        for (int x = 0; x < DOOMGENERIC_RESX; x++)
        {
            const int x2 = ((x * x_ratio) >> 16);
            const int y2_x2_colors = y2_xsource + x2;
            const int i_x_colors = i_xdest + x;

            DG_ScreenBuffer[i_x_colors] = colors[I_VideoBuffer[y2_x2_colors]];
        }
    }

    DG_DrawFrame();
}

//
// I_ReadScreen
//
void I_ReadScreen(byte *scr)
{
    memcpy(scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

//
// I_SetPalette
//
#define GFX_RGB565(r, g, b)                                                    \
    ((((r & 0xF8) >> 3) << 11) | (((g & 0xFC) >> 2) << 5) | ((b & 0xF8) >> 3))
#define GFX_RGB565_R(color) ((0xF800 & color) >> 11)
#define GFX_RGB565_G(color) ((0x07E0 & color) >> 5)
#define GFX_RGB565_B(color) (0x001F & color)

#define GFX_ARGB(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define GFX_RGBA(r, g, b, a) (((r) << 24) | ((g) << 16) | ((b) << 8) | (a))
#define GFX_ABGR(r, g, b, a) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
#define GFX_BGRA(r, g, b, a) (((b) << 24) | ((g) << 16) | ((r) << 8) | (a))

void I_SetPalette(byte *palette)
{
    for (int i = 0; i < 256; i++)
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

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex(int r, int g, int b)
{
    int best, best_diff, diff;
    int i;
    col_t color;

    doomgeneric_printf("I_GetPaletteIndex\n");

    best = 0;
    best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
        color.r = GFX_RGB565_R(colors[i]);
        color.g = GFX_RGB565_G(colors[i]);
        color.b = GFX_RGB565_B(colors[i]);

        diff = (r - color.r) * (r - color.r) + (g - color.g) * (g - color.g) +
               (b - color.b) * (b - color.b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

void I_SetWindowTitle(char *title)
{
    DG_SetWindowTitle(title);
}

void I_GraphicsCheckCommandLine(void)
{
}

void I_SetGrabMouseCallback(grabmouse_callback_t func)
{
}

void I_EnableLoadingDisk(void)
{
}

void I_BindVideoVariables(void)
{
}

void I_DisplayFPSDots(boolean dots_on)
{
}

void I_CheckIsScreensaver(void)
{
}
