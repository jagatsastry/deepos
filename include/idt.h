#ifndef IDT_H
#define IDT_H
/*Based partly on forums.osdever.net*/

#include<defs.h>
struct regs
{
    uint64_t r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx ,rax;     
    uint32_t int_no, err_code;   
};

struct regsForSyscall
{
    uint64_t  r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx,rax ;
    uint32_t int_no, err_code;
};

struct regsForPrint{
    uint64_t rax, rcx, rdx, rsi;
};


typedef struct {
   uint16_t   baseLow;
   uint16_t   selector;
   uint08_t  reservedIst;
   //0x1(present)11(dpl)0(zero)1110(type)
   uint08_t   flags;

   uint16_t   baseMid;
   uint32_t   baseHigh;
   uint32_t   reserved;
} __attribute__((packed)) idtEntry;

typedef struct {
   uint16_t   limit;
   uint64_t   base;
} __attribute__((packed)) idtPointer;


void install_isr(uint08_t isrNum, uint64_t funcPtr);

void idtStart(void);

void irq_install_handler(int irq, void (*handler)(struct regs *r));

//void enterToMemory( void *Addr , int typeOfArg);

volatile int enterPressed ;
volatile int curBuffIndex ;
volatile int *keyPressedPtr;
char buff[256];

#endif 
