//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
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
// DESCRIPTION:
//	Gamma correction LUT stuff.
//	Functions to draw patches (by post) directly to screen.
//	Functions to blit a block to the screen.
//

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "i_system.h"

#include "doomtype.h"

#include "deh_str.h"
#include "i_swap.h"
#include "i_video.h"
#include "m_bbox.h"
#include "m_misc.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "config.h"
#ifdef HAVE_LIBPNG
#include <png.h>
#endif

// TODO: There are separate RANGECHECK defines for different games, but this
// is common code. Fix this.
#define RANGECHECK

// Blending table used for fuzzpatch, etc.
// Only used in Heretic/Hexen

byte *tinttable = NULL;

// villsa [STRIFE] Blending table used for Strife
byte *xlatab = NULL;

// The screen buffer that the v_video.c code draws to.

static byte *dest_screen = NULL;

int dirtybox[4];

// haleyjd 08/28/10: clipping callback function for patches.
// This is needed for Chocolate Strife, which clips patches to the screen.
static vpatchclipfunc_t patchclip_callback = NULL;

//
// V_MarkRect
//
void V_MarkRect(int x, int y, int width, int height)
{
    // If we are temporarily using an alternate screen, do not
    // affect the update box.

    if (dest_screen == I_VideoBuffer)
    {
        M_AddToBox(dirtybox, x, y);
        M_AddToBox(dirtybox, x + width - 1, y + height - 1);
    }
}

//
// V_CopyRect
//
void V_CopyRect(int srcx, int srcy, byte *source, int width, int height,
                int destx, int desty)
{
    byte *src;
    byte *dest;

#ifdef RANGECHECK
    if (srcx < 0 || srcx + width > SCREENWIDTH || srcy < 0 ||
        srcy + height > SCREENHEIGHT || destx < 0 ||
        destx + width > SCREENWIDTH || desty < 0 ||
        desty + height > SCREENHEIGHT)
    {
        I_Error("Bad V_CopyRect");
    }
#endif

    V_MarkRect(destx, desty, width, height);

    src = source + SCREENWIDTH * srcy + srcx;
    dest = dest_screen + SCREENWIDTH * desty + destx;

    for (; height > 0; height--)
    {
        memcpy(dest, src, width);
        src += SCREENWIDTH;
        dest += SCREENWIDTH;
    }
}

//
// V_DrawPatch
// Masks a column based masked pic to the screen.
//

void V_DrawPatch(int x, int y, patch_t *patch)
{
    int count;
    int col;
    column_t *column;
    byte *desttop;
    byte *dest;
    byte *source;
    int w;

    y -= SHORT(patch->topoffset);
    x -= SHORT(patch->leftoffset);

    // haleyjd 08/28/10: Strife needs silent error checking here.
    if (patchclip_callback)
    {
        if (!patchclip_callback(patch, x, y))
            return;
    }

#ifdef RANGECHECK
    if (x < 0 || x + SHORT(patch->width) > SCREENWIDTH || y < 0 ||
        y + SHORT(patch->height) > SCREENHEIGHT)
    {
        I_Error("Bad V_DrawPatch x=%i y=%i patch.width=%i patch.height=%i "
                "topoffset=%i leftoffset=%i",
                x, y, patch->width, patch->height, patch->topoffset,
                patch->leftoffset);
    }
#endif

    V_MarkRect(x, y, SHORT(patch->width), SHORT(patch->height));

    col = 0;
    desttop = dest_screen + y * SCREENWIDTH + x;

    w = SHORT(patch->width);

    for (; col < w; x++, col++, desttop++)
    {
        column = (column_t *) ((byte *) patch + LONG(patch->columnofs[col]));

        // step through the posts in a column
        while (column->topdelta != 0xff)
        {
            source = (byte *) column + 3;
            dest = desttop + column->topdelta * SCREENWIDTH;
            count = column->length;

            while (count--)
            {
                *dest = *source++;
                dest += SCREENWIDTH;
            }
            column = (column_t *) ((byte *) column + column->length + 4);
        }
    }
}

//
// V_DrawPatchFlipped
// Masks a column based masked pic to the screen.
// Flips horizontally, e.g. to mirror face.
//

void V_DrawPatchFlipped(int x, int y, patch_t *patch)
{
    int count;
    int col;
    column_t *column;
    byte *desttop;
    byte *dest;
    byte *source;
    int w;

    y -= SHORT(patch->topoffset);
    x -= SHORT(patch->leftoffset);

    // haleyjd 08/28/10: Strife needs silent error checking here.
    if (patchclip_callback)
    {
        if (!patchclip_callback(patch, x, y))
            return;
    }

#ifdef RANGECHECK
    if (x < 0 || x + SHORT(patch->width) > SCREENWIDTH || y < 0 ||
        y + SHORT(patch->height) > SCREENHEIGHT)
    {
        I_Error("Bad V_DrawPatchFlipped");
    }
#endif

    V_MarkRect(x, y, SHORT(patch->width), SHORT(patch->height));

    col = 0;
    desttop = dest_screen + y * SCREENWIDTH + x;

    w = SHORT(patch->width);

    for (; col < w; x++, col++, desttop++)
    {
        column =
            (column_t *) ((byte *) patch + LONG(patch->columnofs[w - 1 - col]));

        // step through the posts in a column
        while (column->topdelta != 0xff)
        {
            source = (byte *) column + 3;
            dest = desttop + column->topdelta * SCREENWIDTH;
            count = column->length;

            while (count--)
            {
                *dest = *source++;
                dest += SCREENWIDTH;
            }
            column = (column_t *) ((byte *) column + column->length + 4);
        }
    }
}

//
// V_DrawPatchDirect
// Draws directly to the screen on the pc.
//

void V_DrawPatchDirect(int x, int y, patch_t *patch)
{
    V_DrawPatch(x, y, patch);
}

//
// V_DrawBlock
// Draw a linear block of pixels into the view buffer.
//

void V_DrawBlock(int x, int y, int width, int height, byte *src)
{
    byte *dest;

#ifdef RANGECHECK
    if (x < 0 || x + width > SCREENWIDTH || y < 0 || y + height > SCREENHEIGHT)
    {
        I_Error("Bad V_DrawBlock");
    }
#endif

    V_MarkRect(x, y, width, height);

    dest = dest_screen + y * SCREENWIDTH + x;

    while (height--)
    {
        memcpy(dest, src, width);
        src += width;
        dest += SCREENWIDTH;
    }
}

void V_DrawFilledBox(int x, int y, int w, int h, int c)
{
    uint8_t *buf, *buf1;
    int x1, y1;

    buf = I_VideoBuffer + SCREENWIDTH * y + x;

    for (y1 = 0; y1 < h; ++y1)
    {
        buf1 = buf;

        for (x1 = 0; x1 < w; ++x1)
        {
            *buf1++ = c;
        }

        buf += SCREENWIDTH;
    }
}

void V_DrawHorizLine(int x, int y, int w, int c)
{
    uint8_t *buf;
    int x1;

    buf = I_VideoBuffer + SCREENWIDTH * y + x;

    for (x1 = 0; x1 < w; ++x1)
    {
        *buf++ = c;
    }
}

void V_DrawVertLine(int x, int y, int h, int c)
{
    uint8_t *buf;
    int y1;

    buf = I_VideoBuffer + SCREENWIDTH * y + x;

    for (y1 = 0; y1 < h; ++y1)
    {
        *buf = c;
        buf += SCREENWIDTH;
    }
}

void V_DrawBox(int x, int y, int w, int h, int c)
{
    V_DrawHorizLine(x, y, w, c);
    V_DrawHorizLine(x, y + h - 1, w, c);
    V_DrawVertLine(x, y, h, c);
    V_DrawVertLine(x + w - 1, y, h, c);
}

// Restore screen buffer to the i_video screen buffer.

void V_RestoreBuffer(void)
{
    dest_screen = I_VideoBuffer;
}

//
// SCREEN SHOTS
//

typedef struct
{
    char manufacturer;
    char version;
    char encoding;
    char bits_per_pixel;

    unsigned short xmin;
    unsigned short ymin;
    unsigned short xmax;
    unsigned short ymax;

    unsigned short hres;
    unsigned short vres;

    unsigned char palette[48];

    char reserved;
    char color_planes;
    unsigned short bytes_per_line;
    unsigned short palette_type;

    char filler[58];
    unsigned char data; // unbounded
} PACKEDATTR pcx_t;

//
// WritePCXfile
//

void WritePCXfile(char *filename, byte *data, int width, int height,
                  byte *palette)
{
    int i;
    int length;
    pcx_t *pcx;
    byte *pack;

    pcx = Z_Malloc(width * height * 2 + 1000, PU_STATIC, NULL);

    pcx->manufacturer = 0x0a; // PCX id
    pcx->version = 5;         // 256 color
    pcx->encoding = 1;        // uncompressed
    pcx->bits_per_pixel = 8;  // 256 color
    pcx->xmin = 0;
    pcx->ymin = 0;
    pcx->xmax = SHORT(width - 1);
    pcx->ymax = SHORT(height - 1);
    pcx->hres = SHORT(width);
    pcx->vres = SHORT(height);
    memset(pcx->palette, 0, sizeof(pcx->palette));
    pcx->color_planes = 1; // chunky image
    pcx->bytes_per_line = SHORT(width);
    pcx->palette_type = SHORT(2); // not a grey scale
    memset(pcx->filler, 0, sizeof(pcx->filler));

    // pack the image
    pack = &pcx->data;

    for (i = 0; i < width * height; i++)
    {
        if ((*data & 0xc0) != 0xc0)
            *pack++ = *data++;
        else
        {
            *pack++ = 0xc1;
            *pack++ = *data++;
        }
    }

    // write the palette
    *pack++ = 0x0c; // palette ID byte
    for (i = 0; i < 768; i++)
        *pack++ = *palette++;

    // write output file
    length = pack - (byte *) pcx;
    M_WriteFile(filename, pcx, length);

    Z_Free(pcx);
}

//
// V_ScreenShot
//

void V_ScreenShot(char *format)
{
    int i;
    char lbmname[16]; // haleyjd 20110213: BUG FIX - 12 is too small!
    char *ext;

    // find a file name to save it to

#ifdef HAVE_LIBPNG
    extern int png_screenshots;
    if (png_screenshots)
    {
        ext = "png";
    }
    else
#endif
    {
        ext = "pcx";
    }

    for (i = 0; i <= 99; i++)
    {
        M_snprintf(lbmname, sizeof(lbmname), format, i, ext);

        if (!M_FileExists(lbmname))
        {
            break; // file doesn't exist
        }
    }

    if (i == 100)
    {
        I_Error("V_ScreenShot: Couldn't create a PCX");
    }

    {
        // save the pcx file
        WritePCXfile(lbmname, I_VideoBuffer, SCREENWIDTH, SCREENHEIGHT,
                     W_CacheLumpName(DEH_String("PLAYPAL"), PU_CACHE));
    }
}

void V_UseBuffer(byte *buffer)
{
    dest_screen = buffer;
}
