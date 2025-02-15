#include "doomgeneric_syscall.h"
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