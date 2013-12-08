#include <defs.h>
#include <stdio.h>

/*Returns a free block*/
void* sbrk() {
   if (DEBUG) printf("In sbrk");
    uint64_t ret = 100;
    __asm__ __volatile__ ( 
               "movq $0x8, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&ret):"rbx","rdx", "memory") ;
    return (void*)ret;
}
