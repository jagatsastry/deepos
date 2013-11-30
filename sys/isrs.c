#include <stdio.h>
#include <pic.h>
#include <defs.h>
#include <stdio.h>
#include <pic.h>
#include <defs.h>
#include <idt.h>

#define INTERRUPT(vector) \
  __asm__(".global x86_64_isr_vector" #vector "\n"\
      "x86_64_isr_vector" #vector ":\n" \
      "    pushq %rax;" \
      "    pushq %rcx;" \
      "    pushq %rdx;" \
      "    pushq %rsi;" \
      "    pushq %rdi;" \
      "    pushq %r8;" \
      "    pushq %r9;" \
      "    pushq %r10;" \
      "    pushq %r11;" \
      "    movq  %rsp,%rdi;" \
      "    addq  $72, %rdi;"  \
      "    call x86_64_handle_isr_vector" #vector ";" \
      "    popq %r11;" \
      "    popq %r10;" \
      "    popq %r9;" \
      "    popq %r8;" \
      "    popq %rdi;" \
      "    popq %rsi;" \
      "    popq %rdx;" \
      "    popq %rcx;" \
      "    popq %rax;" \
  "iretq;")

struct isr_error_stack_frame
{
  uint64_t error;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
};

#define INTR_HANDLER(_intnum_) \
  void x86_64_handle_isr_vector ## _intnum_(struct isr_error_stack_frame *stack) {\
    printf("TRAP# "#_intnum_"!\n");\
    printf("     ERR:%d\n", stack->error);\
    printf("     CS : %x\n",stack->cs);\
    printf("    RIP : %x\n",stack->rip);\
    printf(" RFLAGS : %x\n",stack->rflags);\
    printf("    RSP : %x\n",stack->rsp);\
    printf("     SS : %x\n",stack->ss);\
    while(1);\
  }

INTERRUPT(8);
INTERRUPT(10);
INTERRUPT(11);
INTERRUPT(13);
INTERRUPT(14);

INTERRUPT(6);
INTERRUPT(9);
INTERRUPT(12);
INTERRUPT(15);

INTR_HANDLER(6);
INTR_HANDLER(9);
INTR_HANDLER(12);
INTR_HANDLER(15);


void x86_64_handle_isr_vector8(struct isr_error_stack_frame *stack) {

  printf("DOUBLE FAULT!\n");
  printf("     ERR:%d\n", stack->error);
  printf("     CS : %x\n",stack->cs);
  printf("    RIP : %x\n",stack->rip);
  printf(" RFLAGS : %x\n",stack->rflags);
  printf("    RSP : %x\n",stack->rsp);
  printf("     SS : %x\n",stack->ss);

  while(1);
}

// TODO - what does thispush onto the stack ?
void x86_64_handle_isr_vector10(struct isr_error_stack_frame *stack) {

  printf("Invalid TSS!\n");
  printf("     ERR:%d\n", stack->error);
  printf("     CS : %x\n",stack->cs);
  printf("    RIP : %x\n",stack->rip);
  printf(" RFLAGS : %x\n",stack->rflags);
  printf("    RSP : %x\n",stack->rsp);
  printf("     SS : %x\n",stack->ss);

  while(1);
}

// TODO - what does thispush onto the stack ?
void x86_64_handle_isr_vector11(struct isr_error_stack_frame *stack) {

  printf("SEGMENT NOT PRESENT!\n");
  printf("     ERR:%d\n", stack->error);
  printf("     CS : %x\n",stack->cs);
  printf("    RIP : %x\n",stack->rip);
  printf(" RFLAGS : %x\n",stack->rflags);
  printf("    RSP : %x\n",stack->rsp);
  printf("     SS : %x\n",stack->ss);

  while(1);
}

#define CPU_READ_REG64(_reg_) \
    uint64_t cpu_read_ ## _reg_() { \
      uint64_t ret = 0; \
      __asm__ __volatile__( "movq %%" #_reg_ ", %0" : "=r" (ret) ); \
      return (ret); \
    }

#define CPU_WRITE_REG64(_reg_) \
    void cpu_write_ ## _reg_(uint64_t val) { \
      __asm__ __volatile__( "movq %0, %%" #_reg_ : /* no output */ : "r" (val) ); \
    }

CPU_READ_REG64(rsp)
CPU_READ_REG64(cr0) /* static inline uint64_t cpu_read_cr0() */
CPU_READ_REG64(cr1) /* static inline uint64_t cpu_read_cr1() */
CPU_READ_REG64(cr2) /* static inline uint64_t cpu_read_cr2() */
CPU_READ_REG64(cr3) /* static inline uint64_t cpu_read_cr3() */

CPU_WRITE_REG64(rsp)
CPU_WRITE_REG64(cr3) /* static inline uint64_t cpu_read_cr3() */

void x86_64_handle_isr_vector13(struct isr_error_stack_frame *stack) {

  printf("GENERAL PROTECTION EXCEPTION!\n");
  printf("     ERR:%d\n", stack->error);
  printf("     CS : %x\n",stack->cs);
  printf("    RIP : %x\n",stack->rip);
  printf(" RFLAGS : %x\n",stack->rflags);
  printf("    RSP : %x\n",stack->rsp);
  printf("     SS : %x\n",stack->ss);

  while(1);
}


struct isr_pf_stack_frame
{
  union {

    uint64_t reserved;
    struct {
      unsigned p    : 1;
      unsigned wr   : 1;
      unsigned us   : 1;
      unsigned rsvd   : 1;
      unsigned id   : 1;
    }error;
  }error;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
};



void x86_64_handle_isr_vector14(struct isr_pf_stack_frame *stack) {

  uint64_t vaddr = cpu_read_cr2();
 

  /*
  if(stack->error.error.p && !(pde & PT_PRESENT_FLAG))
    stale_tlb = 0; // real page fault on present status.

  if(stack->error.error.wr && !(pde & PT_WRITABLE_FLAG))
    stale_tlb = 0; // real page fault on writable status.

  if(stack->error.error.us && !(pde & PT_USER_FLAG))
    stale_tlb = 0; // real page fault on user permission.

  if(stale_tlb) {

    cpu_invlpg((uint64_t*)vaddr);
  }
  else {
  */
    printf("PAGE FAULT!\n");
    printf("  Virt-address : %x\n", vaddr);
    printf("       Present : %d\n", stack->error.error.p);
    printf("            id : %d\n", stack->error.error.id);
    printf("            wr : %d\n", stack->error.error.wr);
    printf("          user : %d\n", stack->error.error.us);
    printf("          rsvd : %d\n", stack->error.error.rsvd);
    printf("            CS : %x\n", stack->cs);
    printf("           RIP : %x\n",stack->rip);
    printf("        RFLAGS : %x\n",stack->rflags);
    printf("           RSP : %x\n",stack->rsp);
    printf("            SS : %x\n",stack->ss);
    while(1);
    //HALT("");
//  }
}




    
void print( struct regsForPrint *regs)
{
    printf("Inside system call for print\n");
    char *strPtr = (char *)regs->rdx;
    int len = (int)regs->rcx;
    for( int i =0; i <len; i++ )
    {
           putc(strPtr[i]);
           //strPtr++;
    }
    printf("\nReturn from print"); 
}

void exitSyscall()
{
    printf("Bye\n");
}


void enterToMemory( void *Addr, int typeOfArg)
{
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
    //int* memAddr = (int *)regs->rdx;
    //printf("\nAlok %p",regs->rdx); 
    //printf("\nAbhishek %p",regs->rcx); 
    enterPressed = 0;
    curBuffIndex = 0;
    //while( !enterPressed);
    //enterToMemory((void *)memAddr,1);

}


uint64_t numSysCalls = 3;

static void *syscalls[3] =
{
     print,
     exitSyscall,
     sscanfSystemCall
};


void syscall_handler( struct regsForSyscall * s)
{
       //printf("SysCall handler begins\n");
       //printf("\nSyscall no %x",s->rbx);
       if( s->rbx > numSysCalls )
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
                              " : :"r" (s->rdi), "r" (s->rsi), "r" (s->rdx), "r" (s->rcx), "r" (s->rax),  "r" (location));

}


void fault_handler()
{
    printf("Div by 0");
    for(;;);
}
    
