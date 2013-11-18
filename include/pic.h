#ifndef PIC_H
#define PIC_H

#include <defs.h>

void init_pics(void);
void port_outb(uint16_t port, uint08_t data);
uint08_t port_inb(uint16_t port);

#endif
