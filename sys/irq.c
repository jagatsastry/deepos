/*
 * Bran's Tutorial: osdever.net
 */
#include<pic.h>
#include<idt.h>
#include<stdio.h>

extern void _irq0();
extern void _irq1();

void install_irqs()
{
    printf("Install irqs\n");
    install_isr(32, (uint64_t)_irq0);
    install_isr(33, (uint64_t)_irq1);
}


void irq_handler(struct regs *r, void (*handler)(struct regs*), int int_no)
{
    handler(r);
    if (int_no >= 40)
        port_outb(0xA0, 0x20);

    port_outb(0x20, 0x20);
}

extern void timer_handler(struct regs *r);
void irq0_handler(struct regs *r)
{
    irq_handler(r, timer_handler, 32);
}

extern void keyboard_handler(struct regs *r);
void irq1_handler(struct regs *r)
{
    irq_handler(r, keyboard_handler, 33);
}
