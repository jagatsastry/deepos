# include <stdio.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>
#include <syscall.h>
#include <files.h>


int close( int fd )
{
     volatile int retVal = -1;
     __asm__ __volatile__("movq %0, %%rax;\
                           movq %1, %%rcx;\
                           movq $12, %%rbx;\
                           int $0x80;\
                           "::"g" ((uint64_t)fd), "g"(&retVal) : "rbx", "rax", "rcx");
      return retVal;
}
