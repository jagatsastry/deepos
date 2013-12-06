# include <stdio.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>
# include <files.h>


int open( const char *fname )
{
   volatile int retVal = -1;
   __asm__ __volatile__("movq %0, %%rax;\
                         movq %1, %%rcx;\
                         movq $10, %%rbx;\
                         int $0x80;\
                         "::"g" (fname), "g"(&retVal) : "rbx", "rax", "rcx");
   return retVal;
}
