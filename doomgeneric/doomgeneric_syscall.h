#ifndef DOOMGENERIC_SYSCALL_H
#define DOOMGENERIC_SYSCALL_H

#include <stddef.h>

void doomgeneric_exit(int status);
void doomgeneric_printf(const char *fmt, ...);
void doomgeneric_puts(const char *msg);
void *doomgeneric_malloc(size_t size);
void doomgeneric_free(void *mem);

#endif // DOOMGENERIC_SYSCALL_H
