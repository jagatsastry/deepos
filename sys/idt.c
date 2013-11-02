/*
 * forum.osdever.net
 */

#include<defs.h>
#include<idt.h>
#include<stdio.h>
#include <stdlib.h>

extern void _idt_load(idtPointer *idtPtr);
extern void _isr0();

extern void timer_phase(void);
extern void timer_install();
extern void install_irqs();

void idtStart(void);

idtEntry idt[256];
idtPointer idtp;

void install_isrs() 
{
    install_isr(0, (uint64_t)_isr0);
    return;
}

void install_isr(uint08_t isrNum, uint64_t funcPtr) 
{
    idtEntry isrEnt;
    isrEnt.baseLow = (funcPtr & 0xFFFF);
    isrEnt.baseMid = ((funcPtr >> 16) & 0xFFFF);
    isrEnt.baseHigh = ((funcPtr >> 32) & 0xFFFFFFFF);
    isrEnt.flags = 0x8E;
    isrEnt.selector = 0x8;
    isrEnt.reservedIst = 0;
    isrEnt.reserved = 0;
    
    idt[isrNum] = isrEnt;
}

void idtStart(void) {
   idtp.limit = (sizeof(idtEntry) * 256) - 1;
   idtp.base = (uint64_t)&idt;
   
   memset(&idt, 0, sizeof(idtEntry) * 256);
   _idt_load(&idtp);

  install_isrs();
  install_irqs();
}
