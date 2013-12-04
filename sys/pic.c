/*
 * Chris Stones' shovel OS
 * Alexander Blessing's osdever.net
 */
#include<defs.h>
#include<stdio.h>
#include<pic.h>

void port_outb(uint16_t port, uint08_t data) {
    __asm__ __volatile( "outb %0, %1;"
                    : /* void */
                    : "a" (data), "d" (port));
}

inline uint08_t port_inb(uint16_t port) {

    uint08_t ret;
    __asm__ __volatile__( "inb  %1, %0;"
                          :"=a" (ret)
                          :"d"  (port)   );
    return ret;
}


#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

/* init_pics()
 * init the PICs and remap them
 */
void init_pics()
{
    int pic1 = 0x20, pic2 = 0x28;
    /* send ICW1 */
    port_outb(PIC1, ICW1);
    port_outb(PIC2, ICW1);

    /* send ICW2 */
    port_outb(PIC1 + 1, pic1);   /* remap */
    port_outb(PIC2 + 1, pic2);   /*  pics */

    /* send ICW3 */
    port_outb(PIC1 + 1, 4);  /* IRQ2 -> connection to slave */
    port_outb(PIC2 + 1, 2);

    /* send ICW4 */
    port_outb(PIC1 + 1, ICW4);
    port_outb(PIC2 + 1, ICW4);

    port_outb(PIC1 + 1, 0);
    port_outb(PIC2 + 1, 0);
    printf("PICS Initialized\n");
    __asm__ __volatile("sti");
}
