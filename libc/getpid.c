#include <defs.h>

/*Returns a free block*/
pid_tt getpid() {
    pid_tt pid = 10;
    __asm__ __volatile__ ( 
               "movq $0x9, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&pid):"rbx","rdx", "memory") ;
    return pid;
}
