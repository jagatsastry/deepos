#include <stdio.h>
#include <pic.h>
#include <defs.h>
#include <idt.h>
#include <task.h>

void print( struct regsForSyscall *regs)
{
    char *strPtr = (char *)regs->rdx;
    int i=0;
    //int len = (int)regs->rcx;
    if (DEBUG) printf("%p",strPtr);
    while( strPtr[i] != '\0' )
    {
           putc(strPtr[i]);
           i++; 
    }
}

void exitSyscall(struct regsForSyscall *regs)
{
    kexit(*(uint64_t*)regs->rdx);
}


//void enterToMemory( void *Addr, int typeOfArg)
void enterToMemory( struct regsForSyscall *regs) 
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


void sscanfSystemCall( struct regsForSyscall *regs )
{
    //const char * formatStr = ( const char *)regs->rdx;
    //void* memAddr = (void *)regs->rdx;
    //if (DEBUG) printf("\nAlok %p",regs->rdx); 
    //if (DEBUG) printf("\nAbhishek %p",regs->rcx);
    keyPressedPtr = (int *)regs->rcx; 
    current_task->waiting_for_input = 1;
    enterPressed = 0;
    curBuffIndex = 0;
    //if (DEBUG) printf("\nValue of keyPressedPtr is %p",keyPressedPtr); 
    //if (DEBUG) printf("The sting is %x",buff);
    //enterToMemory(memAddr,1);

}




extern void cpu_write_rcx();

void sys_sleep(struct regsForSyscall * s) {
  if (DEBUG) printf("Sleeping for %ds\n", s->rdx);
  ksleep(*(uint64_t*)s->rdx);
}

void sys_waitpid(struct regsForSyscall * s) {
  pid_t pid = (pid_t)s->rdx;
  pid_t *retPid = (uint32_t*)s->rsi;
  uint32_t *status =  *(uint32_t**)s->rcx;
  *retPid = kwaitpid(pid, status);
}


void sys_fork(struct regsForSyscall * s) {
  uint64_t* rval = (uint64_t*)s->rdx;
  int ret = kfork(s);
  *rval = ret;
  if (DEBUG) printf("Returning from fork\n");
}

extern void* i_virt_alloc();

void sys_sbrk(struct regsForSyscall * s) {
  uint64_t addr = (uint64_t)i_virt_alloc();
  *(uint64_t*)s->rdx = addr;
}

void sys_getpid(struct regsForSyscall * s) {
  *(uint64_t*)s->rdx = current_task->id;
}

void sys_execvpe(struct regsForSyscall * s) {
  char *filename = (char*)s->rdx;
  char **argv = (char**)s->rcx;
  char **argp = (char**)s->rsi;
  int *ret = (int*)s->rsi;

  if (DEBUG) printf("syscall: Running execvpe of %s\n", filename);
  int argc = 0;
  for(; argv[argc]; argc++);

  *ret = kexecvpe(filename, argc, argv, argp);
  if (DEBUG) printf("In syscall-execvpe. Going back\n");
}

#define NUM_SYSCALLS  11
static void *syscalls[11] =
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
       //if (DEBUG) printf("SysCall handler begins\n");
       //if (DEBUG) printf("\nSyscall no %x",s->rbx);
       if( s->rbx > NUM_SYSCALLS )
         return;
       //if (DEBUG) printf("Inside Syscall handler\n");
       void (*location)(struct regsForSyscall*) = 
           (void (*)(struct regsForSyscall*))syscalls[ s->rbx ];
       //if (DEBUG) printf("\nInside fault handler Alok\n");
       //if (DEBUG) printf("\nrax : %p",s->rax);
       //if (DEBUG) printf("\nrbx %p",s->rbx);
       //if (DEBUG) printf("\nrcx : %x",s->rcx);
       //if (DEBUG) printf("\nrdx %p",s->rdx); 
       location(s);
      /* 
      __asm__ __volatile__ ("call *%0;"
                               : : "r" (location) );*/

}

