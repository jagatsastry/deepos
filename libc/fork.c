#include <defs.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t fork() {
    uint64_t ret = 0;

     __asm__ __volatile__ ( 
               "movq $0x04, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&ret):"rbx","rdx", "memory") ;
   pid_t p = getpid();
   if (DEBUG) printf("Returning from fork. Ret %d Current pid %d\n", ret, p);
   return ret;
}
