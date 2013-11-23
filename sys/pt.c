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

extern uint64_t vga_phy_addr;
extern volatile uint64_t vga_virt_addr;

extern void set_page_used_for_addr(uint64_t ptr);

#define PAGE_TABLE_SIZE 512                         /* Entries per table */
#define PAGE_TABLE_BYTES 0x1000   /*Number of bytes used by page table. 4096 = 512 * 64*/
#define PAGE_TABLE_ALIGNLENT 0x1000

/* mask out bits under page size */
#define ALIGN_DOWN(x)  ((x) & ~(PAGE_TABLE_ALIGNLENT-1))

/* next page aligned address */
#define ALIGN_UP(x)     (((x) & (PAGE_TABLE_ALIGNLENT-1)) ? ALIGN_DOWN(x+PAGE_TABLE_ALIGNLENT) : (x))

#define kprintf(x) printf(x)


int dbg_index = 0;
typedef struct  {
  uint64_t *phys_d; //Stores the physical address of next level page dir
} page_directory_t;

uint64_t 
  *temp_pml4e,
  *temp_pdpe,
  *temp_pde, 
  *temp_kern_pt ,
  *temp_vga_pt ;


uint64_t phy_to_virt(uint64_t phy_addr) {
  return kernmem_offset + phy_addr;
}

/*Alocate a page and return its address*/
void* alloc_virt_above_kern() {
  void* addr = (void*)ALIGN_UP(kernmem_offset + kern_physfree);
  set_page_used_for_addr((uint64_t)addr - kernmem_offset);
  kern_physfree += BYTES_PER_PAGE;
  return addr;
}

#define TRUE 1
#define FALSE 0

void* allocate_page(uint64_t *pml4e, uint64_t *phy, BOOL add_to_pt) {
  void* virt = alloc_virt_above_kern();
  //*phy = virt_to_phy((uint64_t)virt);
  //add_to_page_table(pml4e, virt, *phy);
  return virt;
}

/*
void add_to_page_table(uint64_t *pml4e, void* virt, uint64_t phy) {
  uint64_t pdpe_phy = (uint64_t)((pml4e[0x1ff & (addr >> 39)] & MASK));
  if (pdpe == 0ULL) {
    uint64_t* pdpe = allocate_page(pml4e, &pdpe_phy, FALSE);
    
    
  uint64_t* pde = (uint64_t*)((pdpe[0x1ff & (addr >> 30)] & MASK));
  uint64_t* pt = (uint64_t*)((pde[0x1ff & (addr >> 21)] & MASK));

  uint64_t phy_addr = pt[0x1ff & (addr >> 12)];

  return phy_addr & MASK;
}
  */



/*Alocate a page and return its address*/
uint64_t alloc_phys_above_kern() {
  uint64_t addr = ALIGN_UP(kern_physfree);
  set_page_used_for_addr((uint64_t)addr);
  kern_physfree += BYTES_PER_PAGE;
  return addr;
}

/*Bootloader has mapped physbase to kernmem, and physfree to kernmem + physfree*/
uint64_t virt_to_phy(uint64_t virt_addr) {
  return virt_addr - kernmem_offset;
}

void setPds(uint64_t addr, uint64_t *pt) {
  temp_pml4e[0x1ff & (addr >> 39)] = ((uint64_t)temp_pdpe) | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG;
  temp_pdpe[0x1ff & (addr >> 30)] = ((uint64_t)temp_pde) | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;
  temp_pde[0x1ff & (addr >> 21)] = ((uint64_t)pt) | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG;
}


#define MASK (~((1ull<<12) - 1))

uint64_t get_phy_addr(uint64_t addr, uint64_t* pml4e) {
  uint64_t* pdpe = (uint64_t*)(phy_to_virt(pml4e[0x1ff & (addr >> 39)] & MASK));
  uint64_t* pde = (uint64_t*)(phy_to_virt(pdpe[0x1ff & (addr >> 30)] & MASK));
  uint64_t* pt = (uint64_t*)(phy_to_virt(pde[0x1ff & (addr >> 21)] & MASK));
  
  uint64_t phy_addr = pt[0x1ff & (addr >> 12)];

  return phy_addr & MASK;
}

static inline void cpu_write_cr3(uint64_t val) { 
  __asm__ __volatile__( "movq %0, %%cr3" : /* no output */ : "r" (val) ); 
}


void pt_initialise(uint32_t* modulep) {
  

  printf("%d", sizeof (unsigned long int));
  printf("hex 0xffffffff80000000: %x\n", 0xffffffff80000000ull);

  temp_pml4e = (uint64_t*)(alloc_phys_above_kern());
  temp_pdpe = (uint64_t*)((alloc_phys_above_kern()));
  temp_pde = (uint64_t*)((alloc_phys_above_kern()));
  temp_kern_pt = (uint64_t*)((alloc_phys_above_kern()));
  temp_vga_pt = (uint64_t*)((alloc_phys_above_kern()));

  int a,b;
  printf("Addresses kernmem + physfree: %x pts: %x, %x, \n%x, %x, %x, %x, %x\n%x %x %x\n", 
    kernmem_offset + kern_physfree, (uint64_t)temp_pml4e,  (uint64_t)temp_pdpe, 
    (uint64_t)temp_pde, (uint64_t)temp_vga_pt, (uint64_t)temp_kern_pt, (uint64_t)&a, (uint64_t)&b,
    (uint64_t)&temp_pml4e, (uint64_t)&temp_vga_pt, (uint64_t)vga_virt_addr);

  setPds(kernmem_offset, temp_kern_pt);

  uint64_t kernStart =  kern_physbase;

  for (; kernStart <  kern_physfree + BYTES_PER_PAGE ; kernStart += BYTES_PER_PAGE) {
    uint64_t virt_addr = phy_to_virt(kernStart);
    int idx = (0x1ff & (virt_addr >> 12)) ;
    temp_kern_pt[idx] = (kernStart)  | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;
  }
  

  uint64_t temp_vga_virt_addr = vga_phy_addr + (uint64_t)kernmem_offset;
  setPds(temp_vga_virt_addr, temp_kern_pt);

  //temp_vga_pt[0x1FF & (vga_virt_addr >> 12)] = vga_phy_addr | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;
  temp_kern_pt[0x1FF & ((temp_vga_virt_addr) >> 12)] = vga_phy_addr | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;

  temp_kern_pt[0x1FF & (phy_to_virt(((uint64_t)temp_pml4e)) >> 12)] = (uint64_t)temp_pml4e | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;
  temp_kern_pt[0x1FF & (phy_to_virt(((uint64_t)temp_pml4e)) >> 12)] = (uint64_t)temp_pml4e | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;
  temp_kern_pt[0x1FF & (phy_to_virt(((uint64_t)temp_pml4e)) >> 12)] = (uint64_t)temp_pml4e | PT_PRESENT_FLAG | PT_USER_FLAG | PT_WRITABLE_FLAG ;

  printf("From pt: Vga phys addr: %x, vga virt addr: %x\n", get_phy_addr(temp_vga_virt_addr, temp_pml4e), temp_vga_virt_addr);
  printf("Initializing page tables %d\n", ++dbg_index);

  //kern_physfree += (80*25*4*2); /*4 screens, 25 lines each of 80 chars. 2 bytes each (char + props)*/
  vga_virt_addr = temp_vga_virt_addr;
  __asm__("cli");
  cpu_write_cr3((uint64_t)temp_pml4e);
  temp_pml4e = (uint64_t*)phy_to_virt((uint64_t)temp_pml4e);
  __asm__("sti");

    printf("\nIM BACK  %x",  temp_pml4e[0]);
while(1);
  dbg_index++;
  if (dbg_index != 2) 
    goto fuck;
  dbg_index++;
  if (dbg_index != 3) 
    goto fuck;
  //while(1);
  fuck:
    printf("\nIM BACK");
}




