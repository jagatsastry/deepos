#include <defs.h>
#include <files.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void ps(char *options) {
    __asm__ __volatile__ ( 
               "movq $22, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (options):"rbx","rdx", "memory");
}
