#include <stdio.h>
#include <pic.h>
#include <defs.h>
#include <idt.h>
#include <task.h>

void print( struct regsForPrint *regs)
{
    char *strPtr = (char *)regs->rdx;
    int i=0;
    //int len = (int)regs->rcx;
    printf("%p",strPtr);
    while( strPtr[i] != '\0' )
    {
           putc(strPtr[i]);
           i++; 
    }
}

void exitSyscall(struct regsForPrint *regs)
{
    kexit(*(uint64_t*)regs->rdx);
}


//void enterToMemory( void *Addr, int typeOfArg)
void enterToMemory( struct regsForPrint *regs) 
{
      
     void *Addr = (void *)regs->rax;
     int typeOfArg = (int )regs->rcx; 
     if( typeOfArg == 1)
     {
           int *memAddr = (int *)Addr;
           int buffIndex = 0;
           int num = 0;
           while( buffIndex < curBuffIndex )
           {
               num = num * 10 + ( buff[ buffIndex ] - '0');
               buffIndex++;
           }
           *memAddr = num;
     }
     if( typeOfArg == 2){
           char *memAddr = (char *)Addr;
           int buffIndex = 0;
           *memAddr = buff[buffIndex];
     }
     if( typeOfArg == 3){ 
           char *memAddr = (char *)Addr;
           int buffIndex = 0;
           while( buffIndex < curBuffIndex ){//!(buff[ buffIndex ] == 'r')){ 
              memAddr[buffIndex] = buff[buffIndex]; 
              buffIndex++;
           }
     }
}


void sscanfSystemCall( struct regsForPrint *regs )
{
    //const char * formatStr = ( const char *)regs->rdx;
    //void* memAddr = (void *)regs->rdx;
    //printf("\nAlok %p",regs->rdx); 
    //printf("\nAbhishek %p",regs->rcx);
    keyPressedPtr = (int *)regs->rcx; 
    enterPressed = 0;
    curBuffIndex = 0;
    //printf("\nValue of keyPressedPtr is %p",keyPressedPtr); 
    //printf("The sting is %x",buff);
    //enterToMemory(memAddr,1);

}


#define NUM_SYSCALLS  9


extern void cpu_write_rcx();

void sys_sleep(struct regsForPrint * s) {
  printf("Sleeping for %ds\n", s->rdx);
  ksleep(*(uint64_t*)s->rdx);
}

void sys_waitpid(struct regsForPrint * s) {
  pid_t pid = (pid_t)s->rdx;
  pid_t *retPid = (uint32_t*)s->rsi;
  uint32_t *status =  *(uint32_t**)s->rcx;
  *retPid = kwaitpid(pid, status);
}


void sys_fork(struct regsForPrint * s) {
  uint64_t* rval = (uint64_t*)s->rdx;
  int ret = fork();
  *rval = ret;
  printf("Returning from fork\n");
}

extern void* i_virt_alloc();

void sys_sbrk(struct regsForPrint * s) {
  uint64_t addr = (uint64_t)i_virt_alloc();
  *(uint64_t*)s->rdx = addr;
}

void sys_getpid(struct regsForPrint * s) {
  *(uint64_t*)s->rdx = current_task->id;
}

void sys_execvpe(struct regsForPrint * s) {
  cpu_write_rcx(exec(*(char**)s->rdx));
}

static void *syscalls[10] =
{
     print,
     exitSyscall,
     sscanfSystemCall,
     enterToMemory,
     sys_fork,
     sys_execvpe,
     sys_sleep,
     sys_waitpid,
     sys_sbrk,
     sys_getpid
};

void syscall_handler( struct regsForSyscall * s)
{
       //printf("SysCall handler begins\n");
       //printf("\nSyscall no %x",s->rbx);
       if( s->rbx > NUM_SYSCALLS )
         return;
       //printf("Inside Syscall handler\n");
       void *location = syscalls[ s->rbx ];
       //printf("\nInside fault handler Alok\n");
       //printf("\nrax : %p",s->rax);
       //printf("\nrbx %p",s->rbx);
       //printf("\nrcx : %x",s->rcx);
       //printf("\nrdx %p",s->rdx); 
  
      __asm__ __volatile__ ("pushq %1;\
                              pushq %2;\
                              pushq %3;\
                              pushq %4;\
                              movq %%rsp,%%rdi; \
                              call *%5;\
                              popq %%rax;\
                              popq %%rax;\
                              popq %%rax;\
                              popq %%rax;\
                              " : :"r" (s->rdi), "r" (s->rsi), "r" (s->rdx), "r" (s->rcx), "r" (s->rax),  "r" (location): "rcx");

}

