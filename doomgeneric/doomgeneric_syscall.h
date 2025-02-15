#ifndef DOOMGENERIC_SYSCALL_H
#define DOOMGENERIC_SYSCALL_H

#include <stddef.h>
#include <stdio.h>

void doomgeneric_exit(int status);
void doomgeneric_printf(const char *fmt, ...);
void doomgeneric_snprintf(char* restrict buffer, size_t bufsz,
                          const char* restrict format, ...);
void doomgeneric_fprintf(FILE* restrict stream,
                         const char* restrict format, ...);
int doomgeneric_vsnprintf(char* restrict buffer, size_t bufsz,
                           const char* restrict format, va_list vlist);
void doomgeneric_puts(const char *msg);
void *doomgeneric_malloc(size_t size);
void doomgeneric_free(void *mem);

FILE *doomgeneric_fopen(const char* filename, const char* mode);
int doomgeneric_fclose(FILE *stream);
int doomgeneric_fseek(FILE *stream, long offset, int origin);
int doomgeneric_mkdir(const char* path, unsigned mode);
int doomgeneric_mkdir(const char* path, unsigned mode);
size_t doomgeneric_fread(void *restrict buffer, size_t size, size_t count,
                         FILE *restrict stream);

#endif // DOOMGENERIC_SYSCALL_H
