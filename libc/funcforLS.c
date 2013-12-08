# include <stdio.h>
# include <files.h>
# include <sys/tarfs.h>


void funcForLS( const char *str )
{
     //volatile char returnedFileName[100] = {0};
     //volatile char prevFileName[ 100] = {0};
     //locate the directory in the file system
     //get the start address of the directory in
     //our uint64 variable
     //while(1);
     printf("Doing ls for %s\n", str);

     volatile uint64_t i = 0;
     __asm__ __volatile__("movq %0, %%rax;\
                           movq %1, %%rcx;\
                           movq $18, %%rbx;\
                           int $0x80;\
                         "::"g" (str), "g"(&i) : "rbx", "rax", "rcx");
     if( i != -1 ){
        return;
     }
}

