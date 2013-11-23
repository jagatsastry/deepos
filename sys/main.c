#include <defs.h>
#include <stdio.h>
#include <utilities.h>
#include <kernel.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <phy_mem.h>
#include <virt_mem.h>

extern void enter_user_mode();
uint64_t vga_phy_addr;
volatile uint64_t vga_virt_addr;

void start(uint32_t* modulep, void* kernmem_addr, void* physbase, void* physfree)
{


  vga_phy_addr = 0xB8000;
  vga_virt_addr = vga_phy_addr;
  printf("Physbase: %x physfree: %x\n", physbase, physfree);
  printf("Kernmem %x\n",  kernmem_addr);
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  struct smap_t *smap;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);

		}
	}
	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	init_kernel(modulep, kernmem_addr, physbase, physfree);

	while(1);
}


#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char *temp1, *temp2;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	reload_gdt();
	setup_tss();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    &kernmem,
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	for(
		temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)vga_virt_addr;
		*temp1;
		temp1 += 1, temp2 += 2
	) *temp2 = *temp1;
	while(1);
}
