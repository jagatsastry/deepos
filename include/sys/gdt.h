#ifndef _GDT_H
#define _GDT_H

#include <defs.h>

/**
Task-State Segment (TSS)—A segment that holds the processor state associated with a task.
• TSS Selector—A segment selector that references the TSS descriptor located in the GDT.
• Task Register—A register that holds the TSS selector and TSS descriptor for the current task
SEE: Page 39 of amd manual
*/
//TODO:  tss isn't being set anywhere. Only the address is added.
struct tss_t {
	uint32_t reserved;
	uint64_t rsp0;
	uint32_t unused[11]; //We don't use any field other than rsp0
}__attribute__((packed));
extern struct tss_t tss;

/* adapted from Chris Stones, shovelos */

#define GDT_CS        (0x00180000000000)  /*** code segment descriptor ***/
#define GDT_DS        (0x00100000000000)  /*** data segment descriptor ***/

#define C             (0x00040000000000)  /*** conforming ***/
#define DPL0          (0x00000000000000)  /*** descriptor privilege level 0 ***/
#define DPL1          (0x00200000000000)  /*** descriptor privilege level 1 ***/
#define DPL2          (0x00400000000000)  /*** descriptor privilege level 2 ***/
#define DPL3          (0x00600000000000)  /*** descriptor privilege level 3 ***/
#define P             (0x00800000000000)  /*** present ***/
#define L             (0x20000000000000)  /*** long mode ***/
#define D             (0x40000000000000)  /*** default op size ***/
#define W             (0x00020000000000)  /*** writable data segment ***/

extern uint64_t gdt[];

void reload_gdt();
void setup_tss();

#endif
