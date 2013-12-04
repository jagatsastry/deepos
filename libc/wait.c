#include <defs.h>

/*Waits for a pid to end*/
pid_t waitpid(pid_t pid, int* status, int options) {
    pid_t ret = -1;
    __asm__ __volatile__ ( 
               "movq $0x7, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               movq %2, %%rsi;\
               int $0x80;\
               ":: "g"((uint64_t)pid), "g"((uint64_t)&status), "g"((uint64_t)&ret) :"rbx","rdx", "rcx", "rsi", "memory");

    return ret;
}

#include <defs.h>

/*Waits for a child to end*/
pid_t wait(int* status) {
  return waitpid(-1, status, 0);
}
