#ifndef ADDR_H
#define ADDR_H 

#include <defs.h>

extern uint64_t vga_phy_addr;

extern volatile uint64_t vga_virt_addr;

extern uint64_t kernmem_offset, kern_physbase, kern_physfree; /*kernmem_offset + kern_physfree is where freedom starts*/

/*Increment physfree boundary by "increment" and return the older one*/
uint64_t virt_kern_physfree(int increment); 

#endif
