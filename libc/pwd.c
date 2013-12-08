#include <defs.h>
#include <files.h>
#include <utilities.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *sanitize_dir(char *dirname) {
    return dirnameWithEndSlash(getAbsoluteFilePath(pwd(), dirname));
}

int cd(char *dir) {
     dir = sanitize_dir(dir);
     DIR* df = opendir(dir);
     if (df == 0) {
       printf("cd: %s No such file or directory\n", dir);
       return -1;
     }
     closedir(df);
     __asm__( "movq $23, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (dir):"rbx","rdx") ;
    return 0;
}

/*Returns a free block*/
char* pwd() {
    
    char *cwd = malloc(128);
    __asm__ __volatile__ ( 
               "movq $24, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (cwd):"rbx","rdx", "memory") ;
    return (char*)cwd;
}
