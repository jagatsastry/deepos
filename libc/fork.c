#include <defs.h>
#include <stdlib.h>

uint32_t fork() {
    uint64_t ret = 0;
    char *stack_copy = ((char*)sbrk()) + 2048 - 1 ;
    uint64_t rsp = 200;
   __asm__ __volatile__("movq %%rsp, %0;":"=g"(rsp));

    memcpy(stack_copy, (char*)rsp, 2048);
     __asm__ __volatile__ ( 
               "movq $0x04, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&ret):"rbx","rdx", "memory") ;
   if (ret == 0) {
     __asm__ __volatile__( "movq %0, %%rsp ": : "m"((uint64_t)stack_copy) : "memory" );
   } else {
     __asm__ __volatile__( "movq %0, %%rsp ": : "m"(rsp) : "memory" );
   }
   return ret;
}
