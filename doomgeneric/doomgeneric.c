#include <stdio.h>

#include "m_argv.h"

#include "doomgeneric.h"
#include "doomgeneric_syscall.h"

pixel_t *DG_ScreenBuffer = NULL;

void D_DoomMain(void);

void doomgeneric_Create(int argc, char **argv)
{
    // save arguments
    myargc = argc;
    myargv = argv;

    if (!DG_ScreenBuffer)
    {
        DG_ScreenBuffer =
            doomgeneric_malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * sizeof(pixel_t));
    }

    DG_Init();

    D_DoomMain();
}
