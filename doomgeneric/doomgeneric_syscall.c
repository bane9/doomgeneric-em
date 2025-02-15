#include "doomgeneric_syscall.h"
#include "w_file.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

void doomgeneric_exit(int status) {
    exit(status);
}

void doomgeneric_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}

void doomgeneric_puts(const char *msg) {
    puts(msg);
}

void *doomgeneric_malloc(size_t size) {
    return malloc(size);
}

void doomgeneric_free(void *mem) {
    free(mem);
}

FILE *doomgeneric_fopen(const char* filename, const char* mode) {
    return fopen(filename, mode);
}

int doomgeneric_fclose(FILE *stream) {
    return fclose(stream);
}

int doomgeneric_fseek(FILE *stream, long offset, int origin) {
    return fseek(stream, offset, origin);
}

int doomgeneric_mkdir(const char* path, unsigned mode) {
    return mkdir(path, mode);
}

size_t doomgeneric_fread(void *restrict buffer, size_t size, size_t count,
                         FILE *restrict stream) {
    return fread(buffer, size, count, stream);
}
