#include "doomgeneric_syscall.h"
#include "w_file.h"

#ifdef DOOMGENERC_IWAD_MEMMAPPED
#include "dg_iwad_serized.h"
#endif

static wad_file_t *dg_wad_openfile(char *path);
static void dg_wad_closefile(wad_file_t *path);
static size_t dg_wad_read(wad_file_t *file, unsigned int offset, void *buffer,
                          size_t buffer_len);

typedef struct
{
    wad_file_t wad_file;
    FILE *file;
} dg_file_t;

extern wad_file_class_t doomgeneric_wad_file;

static size_t dg_file_length(FILE *file)
{
    size_t length;

    doomgeneric_fseek(file, 0, SEEK_END);

    length = doomgeneric_ftell(file);

    doomgeneric_fseek(file, 0, SEEK_SET);

    return length;
}

static wad_file_t *dg_wad_openfile(char *path)
{
    FILE *f = doomgeneric_fopen(path, "rb");

    if (!f)
    {
        return NULL;
    }

    dg_file_t *file = doomgeneric_malloc(sizeof(*file));

    *file = (dg_file_t) {.wad_file.file_class = &doomgeneric_wad_file,
                         .wad_file.length = dg_file_length(f),
                         .wad_file.mapped = NULL,
                         .file = f};

    return (wad_file_t *) file;
}

static void dg_wad_closefile(wad_file_t *path)
{
    dg_file_t *wad_file = (dg_file_t *) path;

    FILE *f = wad_file->file;

    doomgeneric_fclose(f);

    doomgeneric_free(path);
}

static size_t dg_wad_read(wad_file_t *file, unsigned int offset, void *buffer,
                          size_t buffer_len)
{
    dg_file_t *wad_file = (dg_file_t *) file;

    FILE *f = wad_file->file;

    doomgeneric_fseek(f, offset, SEEK_SET);

    return doomgeneric_fread(buffer, 1, buffer_len, f);
}

static wad_file_t *dg_wad_openfile_mem(char *path);
static void dg_wad_closefile_mem(wad_file_t *path);
static size_t dg_wad_read_mem(wad_file_t *file, unsigned int offset,
                              void *buffer, size_t buffer_len);

static wad_file_t *dg_wad_openfile_mem(char *path)
{
    static wad_file_t wad_file = {.file_class = &doomgeneric_wad_file};

#ifdef DOOMGENERC_IWAD_MEMMAPPED
    wad_file.mapped = (byte *) doom_iwad_ptr;
    wad_file.length = DOOM_SIZE;
#endif

    return &wad_file;
}

static void dg_wad_closefile_mem(wad_file_t *path)
{
}

static size_t dg_wad_read_mem(wad_file_t *file, unsigned int offset,
                              void *buffer, size_t buffer_len)
{
#ifndef DOOMGENERC_IWAD_MEMMAPPED
    return 0;
#else
    memcpy(buffer, doom_iwad_ptr + offset, buffer_len);

    return buffer_len;
#endif
}

#ifdef DOOMGENERC_IWAD_MEMMAPPED
wad_file_class_t doomgeneric_wad_file = {.OpenFile = dg_wad_openfile_mem,
                                         .CloseFile = dg_wad_closefile_mem,
                                         .Read = dg_wad_read_mem};
#else
wad_file_class_t doomgeneric_wad_file = {.OpenFile = dg_wad_openfile,
                                         .CloseFile = dg_wad_closefile,
                                         .Read = dg_wad_read};
#endif
