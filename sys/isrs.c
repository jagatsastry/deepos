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

void fault_handler()
{
      printf("Div by 0");
          for(;;);
}





