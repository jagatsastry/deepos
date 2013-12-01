#ifndef _STDLIB_H
#define _STDLIB_H
#include <defs.h>

void exit(int status);
void memset(void *init, char val, int sz);
//assume no overlap

void memcpy(void *dest, void *src, uint64_t num_bytes);

#endif
