# include <stdio.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>
# include <files.h>


void write( int fd, void *buff )
{
     
      //volatile int nBytesRead = 0;
      printf("\nFd is %d",fd); 
      if( fd != 2 && fd != 1 )
      {
           printf("\nInvalid file descriptor for write. Only stdout and stderr can be used for write which is terminal is our case");
           return; 
      } 
      __asm__ __volatile__("movq %0, %%rcx;\
                            movq %1, %%rdx;\
                            movq $19, %%rbx;\
                            int $0x80;\
                           "::"g" ((uint64_t)fd), "g"((void *)buff): "rbx", "rdx", "rcx");
      //return nBytesRead;

}

