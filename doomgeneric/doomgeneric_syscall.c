#include "doomgeneric_syscall.h"
#include "w_file.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void doomgeneric_exit(int status)
{
    exit(status);
}

void doomgeneric_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}

void doomgeneric_snprintf(char *restrict buffer, size_t bufsz,
                          const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, bufsz, format, args);
    va_end(args);
}

void doomgeneric_fprintf(FILE *restrict stream, const char *restrict format,
                         ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}

int doomgeneric_vsnprintf(char *restrict buffer, size_t bufsz,
                          const char *restrict format, va_list vlist)
{
    return vsnprintf(buffer, bufsz, format, vlist);
}

void doomgeneric_puts(const char *msg)
{
    puts(msg);
}

void *doomgeneric_malloc(size_t size)
{
    return malloc(size);
}

void *doomgeneric_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void *doomgeneric_realloc(void *ptr, size_t new_size)
{
    return realloc(ptr, new_size);
}

void doomgeneric_free(void *mem)
{
    free(mem);
}

char *doomgeneric_strdup(const char *str1)
{
    return strdup(str1);
}

FILE *doomgeneric_fopen(const char *filename, const char *mode)
{
    return fopen(filename, mode);
}

int doomgeneric_fclose(FILE *stream)
{
    return fclose(stream);
}

int doomgeneric_fseek(FILE *stream, long offset, int origin)
{
    return fseek(stream, offset, origin);
}

int doomgeneric_mkdir(const char *path, unsigned mode)
{
    return mkdir(path, mode);
}

long doomgeneric_ftell(FILE *stream)
{
    return ftell(stream);
}

size_t doomgeneric_fread(void *restrict buffer, size_t size, size_t count,
                         FILE *restrict stream)
{
    return fread(buffer, size, count, stream);
}

size_t doomgeneric_fwrite(const void *restrict buffer, size_t size,
                          size_t count, FILE *restrict stream)
{
    return fwrite(buffer, size, count, stream);
}

int doomgeneric_strcasecmp(const char *s1, const char *s2)
{
    while (1)
    {
        int c1 = tolower(*(unsigned char *) s1);
        int c2 = tolower(*(unsigned char *) s2);

        if (c1 != c2)
        {
            return c1 - c2;
        }

        if (c1 == '\0')
        {
            return 0;
        }

        s1++;
        s2++;
    }
}

int doomgeneric_strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0)
    {
        return 0;
    }

    while (n-- > 0)
    {
        int c1 = tolower(*(unsigned char *) s1);
        int c2 = tolower(*(unsigned char *) s2);

        if (c1 != c2)
        {
            return c1 - c2;
        }

        if (c1 == '\0')
        {
            break;
        }

        s1++;
        s2++;
    }

    return 0;
}
