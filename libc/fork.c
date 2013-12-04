#include <defs.h>
#include <stdio.h>
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
     printf("In child fork\n");
     __asm__ __volatile__( "movq %0, %%rsp ": : "m"((uint64_t)stack_copy) : "memory" );
   } else {
     printf("In parent fork\n");
     __asm__ __volatile__( "movq %0, %%rsp ": : "m"(rsp) : "memory" );
   }
   pid_t p = getpid();
   printf("Returning from fork. Ret %d Current pid %d\n", ret, p);
   while(1);
   return ret;
}
