# include <stdio.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>
# include <files.h>

int read( int fd, void *buff, int nBytes )
{
      volatile int nBytesRead = 0;
      __asm__ __volatile__("movq %0, %%rcx;\
                            movq %1, %%rdx;\
                            movq %2, %%rax;\
                            movq %3, %%r10;\
                            movq $11, %%rbx;\
                            int $0x80;\
                           "::"g" ((uint64_t)fd), "g"((uint64_t)nBytes), "g"((void *)buff), "g"(&nBytesRead): "rbx", "rax", "rcx","r10");
      return nBytesRead;

}
