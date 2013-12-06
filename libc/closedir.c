# include <stdio.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>
# include <files.h>



int closedir( DIR *dir ){
    volatile int returnStatus = 0;
    __asm__ __volatile__("movq %0, %%rax;\
                         movq %1, %%rcx;\
                         movq $15 , %%rbx;\
                         int $0x80;\
                         "::"g" ((uint64_t)dir->fd), "g"(&returnStatus) : "rbx", "rax", "rcx");
    return returnStatus;
}
