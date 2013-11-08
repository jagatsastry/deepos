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

int dbg_index = 0;
uint64_t 
  temp_pml4e[512],
  temp_pdpe[512],
  temp_pde[512] ,
  temp_kern_pt[512] ,
  temp_vga_pt[512] ;

void setPds(uint64_t addr, uint64_t *pt) {
  temp_pml4e[0x1ff & (addr >> 39)] = ((uint64_t)temp_pdpe - kernmem_offset) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
  temp_pdpe[0x1ff & (addr >> 30)] = ((uint64_t)temp_pde - kernmem_offset) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG ;
  temp_pde[0x1ff & (addr >> 21)] = ((uint64_t)pt  - kernmem_offset) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
  /*
  temp_pml4e[0x1ff & (addr >> 39)] = ((uint64_t)temp_pdpe) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
  temp_pdpe[0x1ff & (addr >> 30)] = ((uint64_t)temp_pde ) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG ;
  temp_pde[0x1ff & (addr >> 21)] = ((uint64_t)pt ) | PT_PRESENT_FLAG | PT_WRITABLE_FLAG;
  */
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
  int a = 0;
  int b = 0;
  /*
  temp_pml4e = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_pdpe  = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_pde = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_kern_pt = (uint64_t*)phys_kern_physfree(512 * 8);
  temp_vga_pt = (uint64_t*)phys_kern_physfree(512 * 8);
  */

  printf("Addresses kernmem + physfree: %x pts: %x, %x, %x, %x, %x, %x, %x\n", 
    kernmem_offset + kern_physfree, (uint64_t)temp_pml4e, (uint64_t)temp_pdpe, (uint64_t)temp_pde, (uint64_t)temp_vga_pt, (uint64_t)temp_kern_pt, (uint64_t)&a, (uint64_t)&b);

  kernel_page_tables.pml4e = temp_pml4e;

  kern_physfree = 0x7FDFF180 - kernmem_offset;


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
  kernel_page_tables.pml4e = (uint64_t*)((uint64_t)temp_pml4e - kernmem_offset);

  cpu_write_cr3((uint64_t)kernel_page_tables.pml4e);
  while(1);
}




