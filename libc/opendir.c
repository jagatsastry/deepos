# include <stdio.h>
# include <stdlib.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>
# include <files.h>

DIR* opendir( const char *dirname )
{
  char *dir1 = NULL; 
   if (dirname[strlen((char*)dirname) - 1] != '/') {
     dir1 = strcat((char*)dirname, "/");
   }
   uint64_t dirAddr = (uint64_t)(dir1?dir1:dirname);
   volatile int i = -1;
   DIR  *dir = (DIR *)sbrk(); 
   __asm__ __volatile__("movq %0, %%rax;\
                         movq %1, %%rcx;\
                         movq %2, %%rdx;\
                         movq $13, %%rbx;\
                         int $0x80;\
                         "::"g" (dirAddr), "g"(&i),"g"(dir): "rbx", "rax", "rcx","rdx");
   if (DEBUG) printf("\n Modified value of i is %d:",i);
   if( i != -1 ){
      //printf("Value set is %d",dirIntArr[i]); 
      return dir;
   }
   return 0;
}
