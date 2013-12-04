#include <defs.h>

/*Returns a free block*/
pid_t getpid() {
    pid_t pid = 10;
    __asm__ __volatile__ ( 
               "movq $0x9, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&pid):"rbx","rdx", "memory") ;
    return pid;
}
