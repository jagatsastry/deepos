# include <stdarg.h> 
# include <stdio.h>
# include <idt.h>
#include <defs.h>

int scanf( const char *format, ...){
     va_list argp;
       va_start( argp, format );
       //int numCharactersRead = 0;
       //int* addrNumCharactersRead = &numCharactersRead;
       int *intPtr ;
       char *cptr;
       volatile int keyPressed = 0;
       char printChar[2];
       while( *format != '\0' ){
           keyPressed = 0;
           switch( *format ){
                 case '%':
                          format++ ;
                          switch( *format ){
                               case 'd':
                                         intPtr =  va_arg( argp, int *);
                                         //*intPtr = 10;
                                         //int *newIntPtr = intPtr;
                                          __asm__ __volatile__("movq %0, %%rax;\
                                                                movq %1, %%rcx;\
                                                                movq $0x02, %%rbx;\
                                                                int $0x80;\
                                                                "::"g" (intPtr), "g"(&keyPressed): "rbx", "rax","rcx");
                                         while( !keyPressed );
                                         __asm__ __volatile__("movq %0, %%rax;\
                                                               movq $0x1, %%rcx;\
                                                               movq $0x03, %%rbx;\
                                                               int $0x80;\
                                                               "::"g" (intPtr): "rax", "rcx","rbx");
                                         //enterToMemory( (void *)intPtr,1 );
                                         //printf("The argument is %s",buff);
                                         break;

                               case 'c':
                                        cptr = va_arg( argp, char *);
                                        __asm__( "movq $0x02, %%rbx;\
                                                  movq %1, %%rcx;\
                                                  movq %0, %%rax;\
                                                  int $0x80;\
                                                  "::"g" (cptr),"g"(&keyPressed) : "rbx", "rax","rcx");
                                        while( !keyPressed );
                                        //enterToMemory( (void *)cptr,2 );
                                        __asm__ __volatile__("movq %0, %%rax;\
                                                              movq $0x2, %%rcx;\
                                                              movq $0x03, %%rbx;\
                                                              int $0x80;\
                                                              "::"g" (cptr): "rax", "rcx","rbx");
                                        break;
                               case 's':
                                        cptr = va_arg( argp, char *);
                                        __asm__("movq $0x02, %%rbx;\
                                                 movq %1, %%rcx;\
                                                movq %0, %%rax;\
                                                 int $0x80;\
                                                 "::"g" (cptr),"g"(&keyPressed) :"rax", "rcx", "rbx");
                                        while( !keyPressed );
                                       //enterToMemory( (void *)cptr,1 );
                                        __asm__ __volatile__("movq %0, %%rax;\
                                                              movq $0x3, %%rcx;\
                                                              movq $0x03, %%rbx;\
                                                              int $0x80;\
                                                              "::"g" (cptr): "rax", "rcx","rbx");
                                      break;
                          }
                          format++;
                          break;
                 default:
                         printChar[0] = *format;
                         printChar[1] = '\0';
                         printf("%s",printChar);
                         format++;
            }
     }
    // va_end( argp );
     return 0;
}
 

