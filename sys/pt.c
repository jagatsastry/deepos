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
#define PAGE_TABLE_BYTES 0x1000   /*Number of bytes used by page table. 4096 = 512 * 64*/
#define PAGE_TABLE_ALIGNLENT 0x1000

/* mask out bits under page size */
#define ALIGN_DOWN(x)  ((x) & ~(PAGE_TABLE_ALIGNLENT-1))

/* next page aligned address */
#define ALIGN_UP(x)     (((x) & (PAGE_TABLE_ALIGNLENT-1)) ? ALIGN_DOWN(x+PAGE_TABLE_ALIGNLENT) : (x))

#define kprintf(x) printf(x)


inline uint64_t mm_page_to_phy(uint64_t page) {
    return page * BYTES_PER_PAGE;
}

inline uint64_t mm_addr_to_page(uint64_t addr) {
    return addr / BYTES_PER_PAGE;
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

/*Returns the page directory entry corresonding to the given virtual address*/
uint64_t virt_to_pde(uint64_t virt) {

  struct page_table_mem *ptm;
  static struct page_table_mem bootloader_pages;

  if(kernel_page_tables.pml4e == 0)
  {
    /* kernel structures are not yet setup!*/
    /* try the boot-loader tables.*/
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


static sint64_t _mmap(uint64_t phy, uint64_t virt, struct page_table_mem *tab) {

  uint64_t *pml4e = 0;
  uint64_t *pdpe = 0; //Non functional
  uint64_t *pde = 0;

//  if(!(pml4e = tab->pml4e) && !(pml4e = tab->pml4e = get_phy_page_table(tab)))
  //  return -1; /* out of memory*/

  /* seek in pml4e to pdpe offset.*/
  pml4e += (0x1ff & ( virt >> 39 ));

  /*** find or allocate the pdpe ***/
  if(!(*PHY_TO_VIRT(pml4e,uint64_t*) & PT_PRESENT_FLAG)) {
    if(!(*PHY_TO_VIRT(pml4e,uint64_t*) = (uint64_t)(pdpe = get_phy_page_table(tab))))
      return -1; /* out of memory*/
/*    if(debug_flag) kprintf("new pdpe at 0x%lx\n",pdpe);*/
  }
  else {
    pdpe = (uint64_t*)ALIGN_DOWN(*PHY_TO_VIRT(pml4e,uint64_t*) );
/*    if(debug_flag) kprintf("existing pdpe at 0x%lx\n",pdpe);*/
  }

  /* point pml4e to pdpe*/
  *PHY_TO_VIRT(pml4e,uint64_t*) |= PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
/*  if(debug_flag) kprintf("pml4e[%d] = 0x%lx\n",0x1ff & ( virt >> 39 ),*PHY_TO_VIRT(pml4e,uint64_t*) );*/

  /* seek in pdpe to pde*/
  pdpe += (0x1ff & (virt >> 30 ));
/*  if(debug_flag) kprintf("pdpe += (%d)\n",0x1ff & ( virt >> 30 ));*/

  /*** find or allocate the pde ***/
  if(!(*PHY_TO_VIRT(pdpe,uint64_t*) & PT_PRESENT_FLAG)) {
    if(!(*PHY_TO_VIRT(pdpe,uint64_t*) = (uint64_t)(pde = get_phy_page_table(tab))))
      return -1; /* out of memory*/
/*    if(debug_flag) kprintf("new pde at 0x%lx\n",pde);*/
  }
  else {
    pde = (uint64_t*)ALIGN_DOWN( *PHY_TO_VIRT(pdpe,uint64_t*) );
/*    if(debug_flag) kprintf("existing pde at 0x%lx\n",pde);*/
  }

  /* point pdpe to pde*/
  *PHY_TO_VIRT(pdpe,uint64_t*) |= PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
/*  if(debug_flag) kprintf("pdpe[%d] = 0x%lx\n",0x1ff & ( virt >> 30 ),*PHY_TO_VIRT(pdpe,uint64_t*) );*/

  /* seek in pde to pte*/
  pde += (0x1ff & (virt >> 21 ));
/*  if(debug_flag) kprintf("pde += (%d)\n",0x1ff & ( virt >> 21 ));*/

  /* point pde to physical address.*/
  *PHY_TO_VIRT(pde,uint64_t*) = phy | PT_PRESENT_FLAG | PT_WRITABLE_FLAG | PT_TERMINAL_FLAG;
/*  if(debug_flag) kprintf("pde[%d] = 0x%lx\n",0x1ff & ( virt >> 21 ),*PHY_TO_VIRT(pde,uint64_t*) );*/

  return 0;
}



/*** unmaps a virtual address, returns its physical address, or NULL on error ***/
static uint64_t* _munmap(uint64_t virt, struct page_table_mem *tab) {

  uint64_t *pml4e;
  uint64_t *pdpe;
  uint64_t *pde;

  /*** find or allocate the pml4e ***/
  if(!(pml4e = tab->pml4e))
    return 0; /* no mapping!*/

  /* seek in pml4e to pdpe offset.*/
  pml4e += (0x1ff & ( virt >> 39 ));

  /*** find the pdpe ***/
  if(!(*PHY_TO_VIRT(pml4e,uint64_t*) & PT_PRESENT_FLAG)) {
    return 0; /* no mapping*/
  }
  else {
    pdpe = (uint64_t*)ALIGN_DOWN(*PHY_TO_VIRT(pml4e,uint64_t*) );
  }

  /* seek in pdpe to pde*/
  pdpe += (0x1ff & (virt >> 30 ));

  /*** find the pde ***/
  if(!(*PHY_TO_VIRT(pdpe,uint64_t*) & PT_PRESENT_FLAG)) {
    return 0; /* no mapping*/
  }
  else {
    pde = (uint64_t*)ALIGN_DOWN( *PHY_TO_VIRT(pdpe,uint64_t*) );
  }

  /* seek in pde to pte*/
  pde += (0x1ff & (virt >> 21 ));

  if(!(*PHY_TO_VIRT(pde,uint64_t*) & PT_PRESENT_FLAG)) {
    return 0; /* no mapping*/
  }

  /* unmap*/
  *PHY_TO_VIRT(pde,uint64_t*) &= ~PT_PRESENT_FLAG;

  return (uint64_t*)ALIGN_DOWN(*PHY_TO_VIRT(pde,uint64_t*));
}

sint64_t mmap(uint64_t phy, uint64_t virt, struct page_table_mem *tab) {

  sint64_t success;

  if(!tab)
    tab = &kernel_page_tables;

  success = _mmap(phy, virt, tab);

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

uint64_t virt_kern_physfree(int increment) {
    return kernmem_offset + (kern_physfree += increment) - increment;
}   

uint64_t phys_kern_physfree(int increment) {
    return  (kern_physfree += increment) - increment;
}   

int dbg_index = 0;
uint64_t *temp_pml4e,
  *temp_pdpe,
  *temp_pde ,
  *temp_kern_pt ,
  *temp_vga_pt ;

void setPds(uint64_t addr, uint64_t *pt) {
  temp_pml4e[0x1ff & (addr >> 39)] = ((uint64_t)temp_pdpe - kernmem_offset) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
  temp_pdpe[0x1ff & (addr >> 30)] = ((uint64_t)temp_pde - kernmem_offset) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG ;
  temp_pde[0x1ff & (addr >> 21)] = ((uint64_t)pt  - kernmem_offset) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
}

#define MASK (~((1ull<<12) - 1))

uint64_t get_phy_addr(uint64_t addr, uint64_t* pml4e) {
  uint64_t* pdpe = (uint64_t*)(pml4e[0x1ff & (addr >> 39)] & MASK);
  uint64_t* pde = (uint64_t*)(pdpe[0x1ff & (addr >> 30)] & MASK);
  uint64_t* pt = (uint64_t*)(pde[0x1ff & (addr >> 21)] & MASK);
  
  printf("addr: %x, pml4e: %x, temp_pml4e: %x\n", addr, (uint64_t)pml4e, (uint64_t)temp_pml4e);
  printf("pdpe: %x, temp_pdpe: %x\n", (uint64_t)pdpe, (uint64_t)temp_pdpe);
  printf("pde: %x, temp_pde: %x\n", (uint64_t)pde, (uint64_t)temp_pde);
  printf("pt: %x, temp_kern_pt: %x, temp_vga_pt: %x\n", (uint64_t)pt, (uint64_t)temp_kern_pt, (uint64_t)temp_vga_pt);

  uint64_t phy_addr = pt[0x1ff & (addr >> 12)];

    printf("phy: %x\n", phy_addr);

  return phy_addr & MASK;
  //return phy_addr & (1<<12);
}

void print_vp(uint64_t* virt_addr, char *key) {
      printf("%s: virt %x bootloader phy: %x kernel phy: %x\n", 
        key,
        (uint64_t)virt_addr, 
        get_phy_addr((uint64_t)virt_addr, bootloader_page_tables.pml4e), 
        (uint64_t)virt_addr - kernmem_offset);
}

void pt_initialise(uint32_t* modulep) {
  bootloader_page_tables.pml4e = (uint64_t*)cpu_read_cr3();

  print_vp((uint64_t*)(kernmem_offset + kern_physbase),  "test");
  temp_pml4e = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_pdpe  = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_pde = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_kern_pt = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_vga_pt = (uint64_t*)phys_kern_physfree(512 * 8);


  kernel_page_tables.pml4e = temp_pml4e;


  print_vp(temp_pml4e,  "pml4e");
  print_vp(temp_pdpe,  "pdpe");
  print_vp(temp_pde,  "pde");
  print_vp(temp_kern_pt,  "kern_pt");
  print_vp(temp_vga_pt,  "vga_pt");

  setPds(kernmem_offset + kern_physbase, temp_kern_pt);

  uint64_t kernStart =  kern_physbase;
  printf("%x\n", kernStart);
  printf("%x %d\n", kern_physfree, kern_physfree);
  printf("k %x %x\n", kern_physbase, kern_physfree);
  printf("k %x %x\n", kernmem_offset + kern_physbase, kernmem_offset + kern_physfree);
  for (; kernStart <  kern_physfree; kernStart += BYTES_PER_PAGE) {
    //mmap(kernStart, kernmem_offset + kernStart, &kernel_page_tables);
    int idx = (0x1ff & (( kernmem_offset + kernStart) >> 12)) ;
    temp_kern_pt[idx] = (kernStart)  | PT_PRESENT_FLAG | PT_WRITABLE_FLAG | PT_TERMINAL_FLAG;
    //if (idx < 10)
  }
  int i = 0;
  //for (i = 0; i < 512; i++)
  //  printf("%d %x ", i, temp_kern_pt[i]);
  
  /*
  printf("From pt: Kernmem + physbase: %x, physbase: %x\n", get_phy_addr(kernmem_offset + kern_physbase), kern_physbase);
  printf("From pt: Kernmem + physfree: %x, physfree: %x\n", get_phy_addr(kernmem_offset + kern_physfree), kern_physfree);
  */

  setPds(vga_virt_addr, temp_vga_pt);
  printf("pde vga: %x, temp_vga_pt: %x\n", temp_pde[0x1ff && (vga_virt_addr >> 21)], (uint64_t)temp_vga_pt);
  /*
  for (i = 0; i < 80*25*4*2; i+= BYTES_PER_PAGE)
      mmap((uint64_t)(vga_virt_addr + i), (uint64_t)(kernmem_offset + kern_physfree + i), &kernel_page_tables);
      */
  for (i = 0; i < 0x100000; i += BYTES_PER_PAGE) {
    int idx = 0x1ff & (i >> 12) ;
    temp_vga_pt[idx] = i  | PT_PRESENT_FLAG | PT_WRITABLE_FLAG | PT_TERMINAL_FLAG;
  }

  int k = 0;
  for (i = 0xB8000; i < 0xB8000ull + 25 * 80 * 2 * 4; i += BYTES_PER_PAGE) {
      if (k++ < 3)
        printf("VGA Virt: %x bootloader phy: %x kernel phy: %x\n",  i, get_phy_addr(i, bootloader_page_tables.pml4e), 
        get_phy_addr(i, temp_pml4e));
  }

  for (i = kern_physbase; i < kern_physfree; i+= BYTES_PER_PAGE) {
  //  printf("Virtual %x : Physical - Before: %x After: %x\n", kernmem_offset + i, i, get_phy_addr(kernmem_offset + i));
    if (++k == 10) break;
  }

  printf("From pt: Vga phys addr: %x, vga virt addr: %x\n", get_phy_addr(vga_virt_addr, temp_pml4e), vga_virt_addr);
  printf("Initializing page tables %d\n", ++dbg_index);

  //kern_physfree += (80*25*4*2); /*4 screens, 25 lines each of 80 chars. 2 bytes each (char + props)*/
  kernel_page_tables.pml4e = (uint64_t*)(temp_pml4e);

  cpu_write_cr3((uint64_t)kernel_page_tables.pml4e);
  while(1);
}




