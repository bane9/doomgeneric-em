//
// Copyright(C) 1993-1996 Id Software, Inc.
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
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>

#include "config.h"

#include "deh_str.h"
#include "doomtype.h"
#include "i_timer.h"
#include "i_video.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"

#include "i_system.h"

#include "w_wad.h"
#include "z_zone.h"

#include "doomgeneric_syscall.h"

#define DEFAULT_RAM 6 /* MiB */
#define MIN_RAM 6     /* MiB */

typedef struct atexit_listentry_s atexit_listentry_t;

struct atexit_listentry_s
{
    atexit_func_t func;
    boolean run_on_error;
    atexit_listentry_t *next;
};

static atexit_listentry_t *exit_funcs = NULL;

void I_AtExit(atexit_func_t func, boolean run_on_error)
{
    atexit_listentry_t *entry;

    entry = doomgeneric_malloc(sizeof(*entry));

    entry->func = func;
    entry->run_on_error = run_on_error;
    entry->next = exit_funcs;
    exit_funcs = entry;
}

// Zone memory auto-allocation function that allocates the zone size
// by trying progressively smaller zone sizes until one is found that
// works.

static byte *AutoAllocMemory(int *size, int default_ram, int min_ram)
{
    byte *zonemem;

    // Allocate the zone memory.  This loop tries progressively smaller
    // zone sizes until a size is found that can be allocated.
    // If we used the -mb command line parameter, only the parameter
    // provided is accepted.

    zonemem = NULL;

    while (zonemem == NULL)
    {
        // We need a reasonable minimum amount of RAM to start.

        if (default_ram < min_ram)
        {
            I_Error("Unable to allocate %i MiB of RAM for zone", default_ram);
        }

        // Try to allocate the zone memory.

        *size = default_ram * 1024 * 1024;

        zonemem = doomgeneric_malloc(*size);

        // Failed to allocate?  Reduce zone size until we reach a size
        // that is acceptable.

        if (zonemem == NULL)
        {
            default_ram -= 1;
        }
    }

    return zonemem;
}

byte *I_ZoneBase(int *size)
{
    byte *zonemem;
    int min_ram, default_ram;
    int p;

    //!
    // @arg <mb>
    //
    // Specify the heap size, in MiB (default 16).
    //

    p = M_CheckParmWithArgs("-mb", 1);

    if (p > 0)
    {
        default_ram = atoi(myargv[p + 1]);
        min_ram = default_ram;
    }
    else
    {
        default_ram = DEFAULT_RAM;
        min_ram = MIN_RAM;
    }

    zonemem = AutoAllocMemory(size, default_ram, min_ram);

    doomgeneric_printf("zone memory: %p, %x allocated for zone\n", zonemem,
                       *size);

    return zonemem;
}

void I_PrintBanner(char *msg)
{
    int i;
    int spaces = 35 - (strlen(msg) / 2);

    for (i = 0; i < spaces; ++i)
        putchar(' ');

    doomgeneric_puts(msg);
}

void I_PrintDivider(void)
{
    int i;

    for (i = 0; i < 75; ++i)
    {
        putchar('=');
    }

    putchar('\n');
}

void I_PrintStartupBanner(char *gamedescription)
{
    I_PrintDivider();
    I_PrintBanner(gamedescription);
    I_PrintDivider();

    doomgeneric_printf(
        " " PACKAGE_NAME
        " is free software, covered by the GNU General Public\n"
        " License.  There is NO warranty; not even for MERCHANTABILITY or "
        "FITNESS\n"
        " FOR A PARTICULAR PURPOSE. You are welcome to change and distribute\n"
        " copies under certain conditions. See the source for more "
        "information.\n");

    I_PrintDivider();
}

//
// I_ConsoleStdout
//
// Returns true if stdout is a real console, false if it is a file
//

boolean I_ConsoleStdout(void)
{
    return 0;
}

//
// I_Quit
//

void I_Quit(void)
{
    atexit_listentry_t *entry;

    // Run through all exit functions

    entry = exit_funcs;

    while (entry != NULL)
    {
        entry->func();
        entry = entry->next;
    }
}

//
// I_Error
//

static boolean already_quitting = false;

void I_Error(char *error, ...)
{
    char msgbuf[512];
    va_list argptr;
    atexit_listentry_t *entry;
    boolean exit_gui_popup;

    if (already_quitting)
    {
        fprintf(stderr, "Warning: recursive call to I_Error detected.\n");
        doomgeneric_exit(-1);
    }
    else
    {
        already_quitting = true;
    }

    // Message first.
    va_start(argptr, error);
    // fprintf(stderr, "\nError: ");
    vfprintf(stderr, error, argptr);
    fprintf(stderr, "\n\n");
    va_end(argptr);
    fflush(stderr);

    // Write a copy of the message into buffer.
    va_start(argptr, error);
    memset(msgbuf, 0, sizeof(msgbuf));
    M_vsnprintf(msgbuf, sizeof(msgbuf), error, argptr);
    va_end(argptr);

    // Shutdown. Here might be other errors.

    entry = exit_funcs;

    while (entry != NULL)
    {
        if (entry->run_on_error)
        {
            entry->func();
        }

        entry = entry->next;
    }

    doomgeneric_exit(-1);
}
