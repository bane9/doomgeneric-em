
#include "doomgeneric_syscall.h"
#include "w_file.h"

static wad_file_t *dg_wad_openfile(char *path);
static void dg_wad_closefile(wad_file_t *path);
static size_t dg_wad_read(wad_file_t *file, unsigned int offset,
                              void *buffer, size_t buffer_len);

typedef struct {
    wad_file_t wad_file;
    FILE *file;
} dg_file_t;

extern wad_file_class_t doomgeneric_wad_file;

static size_t dg_file_length(FILE* file) {
    size_t length;

    fseek(file, 0, SEEK_END);

    length = ftell(file);

    fseek(file, 0, SEEK_SET);

    return length;
}

static wad_file_t *dg_wad_openfile(char *path) {
    FILE* f = fopen(path, "rb");

    if (!f) {
        return NULL;
    }

    dg_file_t *file = doomgeneric_malloc(sizeof(*file));
    
    *file = (dg_file_t) {
        .wad_file.file_class = &doomgeneric_wad_file,
        .wad_file.length = dg_file_length(f),
        .wad_file.mapped = NULL,
        .file = f
    };

    return (wad_file_t *) file;
}

static void dg_wad_closefile(wad_file_t *path) {
    dg_file_t *wad_file = (dg_file_t *) path;

    FILE *f = wad_file->file;

    fclose(f);

    doomgeneric_free(path);
}

static size_t dg_wad_read(wad_file_t *file, unsigned int offset,
                              void *buffer, size_t buffer_len) {
    dg_file_t *wad_file = (dg_file_t *) file;

    FILE *f = wad_file->file;

    fseek(f, offset, SEEK_SET);

    return fread(buffer, 1, buffer_len, f);
}

static wad_file_t *dg_wad_openfile_mem(char *path);
static void dg_wad_closefile_mem(wad_file_t *path);
static size_t dg_wad_read_mem(wad_file_t *file, unsigned int offset,
                              void *buffer, size_t buffer_len);

static wad_file_t *dg_wad_openfile_mem(char *path) {
    static wad_file_t wad_file = {0};

    return &wad_file;
}

static void dg_wad_closefile_mem(wad_file_t *path) {

}

static size_t dg_wad_read_mem(wad_file_t *file, unsigned int offset,
                              void *buffer, size_t buffer_len) {
#ifndef DOOMGENERIC_WAD_MEMBUFFER
    char *DOOMGENERIC_WAD_MEMBUFFER = NULL; // NOTE: dummy value
#endif

    memcpy(buffer, DOOMGENERIC_WAD_MEMBUFFER + offset, buffer_len);

    return buffer_len;
}

#ifdef DOOMGENERIC_MMAPED_WAD
wad_file_class_t doomgeneric_wad_file = {
    .OpenFile = dg_wad_openfile_mem,
    .CloseFile = dg_wad_closefile_mem,
    .Read = dg_wad_read_mem
};
#else
wad_file_class_t doomgeneric_wad_file = {
    .OpenFile = dg_wad_openfile,
    .CloseFile = dg_wad_closefile,
    .Read = dg_wad_read
};
#endif