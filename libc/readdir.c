# include <stdio.h>
# include <defs.h>
# include <idt.h>
# include <files.h>
# include <string.h>
# include <sys/tarfs.h>

char* readAddr( DIR * dir ){
   //temporary arrangement use a kmalloc
   volatile char returnedFileName[100] = {0};
   if( dir == 0 || dir->eofDirectory ){
       printf("\nkkkk");
       return 0;
   }
   __asm__ __volatile__("movq %0, %%rax;\
                         movq %1, %%rcx;\
                         movq $14, %%rbx;\
                         int $0x80;\
                         "::"g" (dir), "g"(returnedFileName) : "rbx", "rax", "rcx");
   if( *returnedFileName == '\0')
   {
       return 0;
   }else{
      return dir->prevFile;
   }
}
