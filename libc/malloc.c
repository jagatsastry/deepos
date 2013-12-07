#include <defs.h>
#include <stdlib.h>
#include <stdio.h>

void* malloc(size_t size) {
    uint64_t ret = 0;
    __asm__ __volatile__ (
              "movq $17, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               ":: "g"((uint64_t)size), "g"((uint64_t)&ret) :"rbx","rdx", "rcx", "memory");

    return (void*)ret;
}

