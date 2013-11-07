/*
 * pt.c
 *
 *  Created on: Jan 8, 2011
 *      Author: cds
 */

#include <defs.h>
#include <mem.h>
#include <region.h>
#include <stdio.h>
#include <stdlib.h>
#include <pt.h>
#include <addr.h>

#define PAGE_TABLE_SIZE 512                         /* Entries per table */
#define PAGE_TABLE_BYTES 0x1000   //Number of bytes used by page table. 4096 = 512 * 64
#define PAGE_TABLE_ALIGNLENT 0x1000

/* mask out bits under page size */
#define ALIGN_DOWN(x)	((x) & ~(PAGE_TABLE_ALIGNLENT-1))

/* next page aligned address */
#define ALIGN_UP(x)     (((x) & (PAGE_TABLE_ALIGNLENT-1)) ? ALIGN_DOWN(x+PAGE_TABLE_ALIGNLENT) : (x))

#define kprintf(x) printf(x)

extern uint64_t kernmem_offset, kern_physbase, kern_physfree;

inline uint64_t mm_page_to_phy(uint64_t page) {
    return page * PAGE_SIZE;
}

inline uint64_t mm_addr_to_page(uint64_t addr) {
    return addr / PAGE_SIZE;
}


struct page_table_mem kernel_page_tables = {

	NULL,  /* pml4e */
	0,     /* stack top */
	0      /* stack bottom */
};

struct page_table_mem bootloader_page_tables = {

	NULL,    /* pml4e */
	0,       /* stack top */
	0        /* stack bottom */
};

struct page_table_mem* get_page_table_struct() {

	uint64_t *cr3 = (uint64_t*)cpu_read_cr3();

	if(kernel_page_tables.pml4e == cr3)
		return &kernel_page_tables;

	if(bootloader_page_tables.pml4e == cr3)
		return &bootloader_page_tables;

	printf("no page table struct\n");
	return 0;
}


/*** get a physical page for use in the page tables ***/
static uint64_t* get_phy_page_table( struct page_table_mem *tab) {

	uint64_t ret = 0;
/*
	if((tab->table_stack_bottom + PAGE_TABLE_BYTES) > tab->table_stack_top) {

		uint64_t phy_page = mm_phy_alloc_page();

		if(!phy_page)
			goto done;

		tab->table_stack_bottom = mm_page_to_phy( phy_page );
		tab->table_stack_top    = tab->table_stack_bottom + PAGE_SIZE;
	}
	tab->table_stack_top -= PAGE_TABLE_BYTES;

	ret = tab->table_stack_top;

done:
*/
    uint64_t phy_page = mm_phy_alloc_page();
    ret = mm_page_to_phy( phy_page );

	if(ret)
		memset( PHY_TO_VIRT(ret,void*), 0x00, PAGE_TABLE_BYTES);

	return (uint64_t*)ret;
}

//Returns the page directory entry corresonding to the given virtual address
uint64_t virt_to_pde(uint64_t virt) {

	struct page_table_mem *ptm;
	static struct page_table_mem bootloader_pages;

	if(kernel_page_tables.pml4e == 0)
	{
		// kernel structures are not yet setup!
		// try the boot-loader tables.
		memset(&bootloader_pages, 0, sizeof bootloader_pages);
		bootloader_pages.pml4e = (uint64_t*)cpu_read_cr3();
		ptm = &bootloader_pages;
	}
	else
	{
		ptm = get_page_table_struct();
	}

	uint64_t *_pml4e = 0x0000;
	uint64_t *_pdpe  = 0x0000;
	uint64_t *_pde   = 0x0000;


	_pml4e = ptm->pml4e;
	_pml4e  += (0x1ff & (virt >> 39));
	_pml4e  = PHY_TO_VIRT(_pml4e, uint64_t*);

	if(!(*_pml4e & PT_PRESENT_FLAG)) {
		return 0;
	}

	_pdpe  = (uint64_t*)ALIGN_DOWN(*_pml4e);
	_pdpe += (0x1ff & (virt >> 30));
	_pdpe  = PHY_TO_VIRT(_pdpe, uint64_t*);

	if(!(*_pdpe & PT_PRESENT_FLAG)) {
		return 0;
	}

	_pde  = (uint64_t*)ALIGN_DOWN(*_pdpe);
	_pde += (0x1ff & (virt >> 21));
	_pde  = PHY_TO_VIRT(_pde, uint64_t*);


	return *_pde;
}

uint64_t virt_to_phy(uint64_t virt) {

	uint64_t pde = virt_to_pde(virt);

	if(!(pde & PT_PRESENT_FLAG))
		return 0;

	return ALIGN_DOWN(pde);
}

//static BOOL debug_flag = FALSE;

static sint64_t _mmap(uint64_t phy, uint64_t virt, struct page_table_mem *tab) {

	uint64_t *pml4e;
	uint64_t *pdpe;
	uint64_t *pde;

//	if(debug_flag) kprintf("_mmap 0x%lx,0x%lx\n",phy,virt);

	/*** find or allocate the pml4e ***/

	/*** find or allocate the pml4e ***/

	/*** find or allocate the pml4e ***/

	/*** find or allocate the pml4e ***/
	if(!(pml4e = tab->pml4e) && !(pml4e = tab->pml4e = get_phy_page_table(tab)))
		return -1; // out of memory

	// seek in pml4e to pdpe offset.
	//if(debug_flag) kprintf("pml4e += (%d)\n",0x1ff & ( virt >> 39 ));
	pml4e += (0x1ff & ( virt >> 39 ));

	/*** find or allocate the pdpe ***/
	if(!(*PHY_TO_VIRT(pml4e,uint64_t*) & PT_PRESENT_FLAG)) {
		if(!(*PHY_TO_VIRT(pml4e,uint64_t*) = (uint64_t)(pdpe = get_phy_page_table(tab))))
			return -1; // out of memory
//		if(debug_flag) kprintf("new pdpe at 0x%lx\n",pdpe);
	}
	else {
		pdpe = (uint64_t*)ALIGN_DOWN(*PHY_TO_VIRT(pml4e,uint64_t*) );
//		if(debug_flag) kprintf("existing pdpe at 0x%lx\n",pdpe);
	}

	// point pml4e to pdpe
	*PHY_TO_VIRT(pml4e,uint64_t*) |= PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
//	if(debug_flag) kprintf("pml4e[%d] = 0x%lx\n",0x1ff & ( virt >> 39 ),*PHY_TO_VIRT(pml4e,uint64_t*) );

	// seek in pdpe to pde
	pdpe += (0x1ff & (virt >> 30 ));
//	if(debug_flag) kprintf("pdpe += (%d)\n",0x1ff & ( virt >> 30 ));

	/*** find or allocate the pde ***/
	if(!(*PHY_TO_VIRT(pdpe,uint64_t*) & PT_PRESENT_FLAG)) {
		if(!(*PHY_TO_VIRT(pdpe,uint64_t*) = (uint64_t)(pde = get_phy_page_table(tab))))
			return -1; // out of memory
//		if(debug_flag) kprintf("new pde at 0x%lx\n",pde);
	}
	else {
		pde = (uint64_t*)ALIGN_DOWN( *PHY_TO_VIRT(pdpe,uint64_t*) );
//		if(debug_flag) kprintf("existing pde at 0x%lx\n",pde);
	}

	// point pdpe to pde
	*PHY_TO_VIRT(pdpe,uint64_t*) |= PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
//	if(debug_flag) kprintf("pdpe[%d] = 0x%lx\n",0x1ff & ( virt >> 30 ),*PHY_TO_VIRT(pdpe,uint64_t*) );

	// seek in pde to pte
	pde += (0x1ff & (virt >> 21 ));
//	if(debug_flag) kprintf("pde += (%d)\n",0x1ff & ( virt >> 21 ));

	// point pde to physical address.
	*PHY_TO_VIRT(pde,uint64_t*) = phy | PT_PRESENT_FLAG | PT_WRITABLE_FLAG | PT_TERMINAL_FLAG;
//	if(debug_flag) kprintf("pde[%d] = 0x%lx\n",0x1ff & ( virt >> 21 ),*PHY_TO_VIRT(pde,uint64_t*) );

	return 0;
}

/*** unmaps a virtual address, returns its physical address, or NULL on error ***/
static uint64_t* _munmap(uint64_t virt, struct page_table_mem *tab) {

	uint64_t *pml4e;
	uint64_t *pdpe;
	uint64_t *pde;

	/*** find or allocate the pml4e ***/
	if(!(pml4e = tab->pml4e))
		return 0; // no mapping!

	// seek in pml4e to pdpe offset.
	pml4e += (0x1ff & ( virt >> 39 ));

	/*** find the pdpe ***/
	if(!(*PHY_TO_VIRT(pml4e,uint64_t*) & PT_PRESENT_FLAG)) {
		return 0; // no mapping
	}
	else {
		pdpe = (uint64_t*)ALIGN_DOWN(*PHY_TO_VIRT(pml4e,uint64_t*) );
	}

	// seek in pdpe to pde
	pdpe += (0x1ff & (virt >> 30 ));

	/*** find the pde ***/
	if(!(*PHY_TO_VIRT(pdpe,uint64_t*) & PT_PRESENT_FLAG)) {
		return 0; // no mapping
	}
	else {
		pde = (uint64_t*)ALIGN_DOWN( *PHY_TO_VIRT(pdpe,uint64_t*) );
	}

	// seek in pde to pte
	pde += (0x1ff & (virt >> 21 ));

	if(!(*PHY_TO_VIRT(pde,uint64_t*) & PT_PRESENT_FLAG)) {
		return 0; // no mapping
	}

	// unmap
	*PHY_TO_VIRT(pde,uint64_t*) &= ~PT_PRESENT_FLAG;

	return (uint64_t*)ALIGN_DOWN(*PHY_TO_VIRT(pde,uint64_t*));
}

sint64_t mmap(uint64_t phy, uint64_t virt, struct page_table_mem *tab) {

	sint64_t success;

	if(!tab)
		tab = &kernel_page_tables;

	success = _mmap(phy,virt,tab);

	if(success != 0)
		kprintf("MMAP FAILED!!!!\n");

	return success;
}

uint64_t *munmap(uint64_t virt, struct page_table_mem *tab) {

	uint64_t *phy;

	if(!tab)
		tab = &kernel_page_tables;

	phy = _munmap(virt,tab);
  return phy;
}

/*
 * and retire the one generated for us by the boot loader.
 */
void pt_initialise(uint32_t* modulep) {

	bootloader_page_tables.pml4e = (uint64_t*)cpu_read_cr3();

	/*** force mapping of first megabyte //Running into error. Possibly because of mem overwrite
  uint64_t b = 0;
	for(; b<PAGE_SIZE * 512; b+=PAGE_SIZE) {

		mmap( b, PHY_TO_VIRT(b,uint64_t), &kernel_page_tables); // at offset

		mmap( b, b, &kernel_page_tables);						// identity map
	}***/


    //while(modulep[0] != 0x9001) modulep += modulep[1]+2;
    //struct mm_phy_reg* smap;
/*    for(smap = (struct mm_phy_reg*)(modulep+2); smap < (struct mm_phy_reg*)((char*)modulep+modulep[1]+2*4); ++smap) {
          if (smap->type == 1  && smap->len != 0) {
              uint64_t b = align_next_page(smap->base);
              uint64_t l = (b== smap->base) ? smap->len: (smap->len - (b- smap->base));
          for(; l>=PAGE_SIZE; l-=PAGE_SIZE, b+=PAGE_SIZE)
            mmap( b, PHY_TO_VIRT(b,uint64_t), &kernel_page_tables);
        }
    }
    printf("Initializing page tables 4\n");
    while(1);
    
*/
  uint64_t kernStart =  kern_physbase;

  for (; kernStart <  kern_physfree; kernStart += PAGE_SIZE) {
    mmap(kernStart, kernmem_offset + kernStart, &kernel_page_tables);
  }

	/*** map all physical memory at virtual = physical + VIRT_OFFSET ***/
/*
  struct mm_phy_reg* r = regs;
	for(; r<regs+regnum; r++) {

		if(r->type != MM_PHY_USABLE)
			continue;

		uint64_t b = r->base & (PAGE_SIZE-1) ? ((r->base + PAGE_SIZE) & PAGE_SIZE)  : r->base;
		uint64_t l = r->base & (PAGE_SIZE-1) ? ( r->len  - (r->base   & PAGE_SIZE)) : r->len;

		for(; l>=PAGE_SIZE; l-=PAGE_SIZE, b+=PAGE_SIZE)
			mmap( b, PHY_TO_VIRT(b,uint64_t), &kernel_page_tables);
	}
*/

	/*** map in the kernel ***/
  /*
  uint64_t v = VIRT_KERNEL_BASE;
	for(; ; v += PAGE_SIZE) {

		uint64_t p = virt_to_phy(v);

		if(p)
			mmap( p, v, &kernel_page_tables);
		else
			break;
	}
  */
    printf("Initializing page tables 5\n");

	/*** identity map low memory ***
	 * we still need access to structures created by the boot loader
	 * ( the stack being the most important one! )
	 */
   /*
  b = 0;
	for(; b<0x100000; b+=PAGE_SIZE)
		mmap( b, b, &kernel_page_tables);
  printf("Initializing page tables 6\n");
  while(1);
*/
  
	/*** load new page tables ***/
   
   int i = 0;
    for (i = 0; i < 80*25*4*2; i+= PAGE_SIZE)
        mmap((uint64_t)(vga_virt_addr + i), (uint64_t)(kernmem_offset + kern_physfree + i), &kernel_page_tables);

  kern_physfree += (80*25*4*2); //4 screens, 25 lines each of 80 chars. 2 bytes each (char + props)

	//cpu_write_cr3((uint64_t)kernel_page_tables.pml4e);
  printf("Initializing page tables 7\n");
  while(1);
}



