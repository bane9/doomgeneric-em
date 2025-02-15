#include "doomgeneric_syscall.h"
#include "w_file.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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
