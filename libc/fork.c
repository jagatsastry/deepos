#include <defs.h>
#include <stdio.h>
#include <stdlib.h>

//char *stack_copy_parent;
//char *stack_copy_child;
uint32_t fork() {
    uint64_t ret = 0;
    char* stack_copy_parent = ((char*)sbrk()) + 2048 - 1 ;
    char* stack_copy_child = ((char*)sbrk()) + 2048 - 1 ;
    uint64_t rsp = 200;
   __asm__ __volatile__("movq %%rsp, %0;":"=g"(rsp));

    memcpy(stack_copy_parent, (char*)rsp, 2048);
    memcpy(stack_copy_child, (char*)rsp, 2048);
     __asm__ __volatile__ ( 
               "movq $0x04, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&ret):"rbx","rdx", "memory") ;
   pid_t p = getpid();
   if (DEBUG) printf("Returning from fork. Ret %d Current pid %d\n", ret, p);
   return ret;
}
