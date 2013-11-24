/*Adapted from Rishabh Sharma's SBU OS. Distriuted under Lesser GPL license*/
#include<stdio.h>
#include<defs.h>
#include <stdlib.h>
#include<virt_mem.h>
#include<phy_mem.h>
#include <utilities.h>
#define ADDITIONAL 0x4800
#define IDENTITY_MAP_VIRTUAL_START 0xFFFFFFFFF0000000UL
#define IDENTITY_MAP_PHYSICAL_START 0x0UL
#define VGA_BUFFER 0xB8000 
extern char kernmem;
extern uint64_t *mem_map;
extern uint64_t vga_virt_addr;

uint64_t temp_virtual_adderss_vga;
uint64_t kern_stack_virt;

struct page_directory_t *kern_pml4e_phy;
struct page_directory_t *kern_pml4e_virt;

struct page_directory_t *cur_pml4e_virt; 

void map_kernel(){
  uint64_t physbase = get_physbase();
  uint64_t physfree = get_physfree();
  kern_pml4e_phy = (struct page_directory_t *) page_alloc();

  struct page_directory_t *pdpt = (struct page_directory_t *) page_alloc();
  
  uint64_t pdpt_entry = (uint64_t)pdpt;
  add_attribute(&pdpt_entry,PRESENT);
  add_attribute(&pdpt_entry,WRITABLE);
  add_attribute(&pdpt_entry,USER_MODE);
  kern_pml4e_phy->entries[PML4E_INDEX((uint64_t)&kernmem)] = pdpt_entry;
  struct page_directory_t *pdt = (struct page_directory_t *) page_alloc();
  uint64_t pdt_entry = (uint64_t)pdt;
  add_attribute(&pdt_entry,PRESENT);
  add_attribute(&pdt_entry,WRITABLE);
  add_attribute(&pdt_entry,USER_MODE);
  pdpt->entries[PDPE_INDEX((uint64_t)&kernmem)] = pdt_entry;
  struct page_directory_t *pt = (struct page_directory_t *) page_alloc();
  uint64_t pt_entry = (uint64_t)pt;
  add_attribute(&pt_entry,PRESENT);
  add_attribute(&pt_entry,WRITABLE);
  add_attribute(&pt_entry,USER_MODE);
  pdt->entries[PDE_INDEX((uint64_t)&kernmem)] = pt_entry;
  uint64_t virtualAddr = (uint64_t)&kernmem;
  physfree = ADDITIONAL + physfree;
 
  while(physbase < physfree)
  {
          uint64_t entry = physbase;
    add_attribute(&entry,PRESENT);
    add_attribute(&entry,WRITABLE);
    add_attribute(&entry,USER_MODE);
    pt->entries[PT_INDEX(virtualAddr)] = entry;
   
    physbase= physbase + 0x1000;
    virtualAddr+=0x1000;
  }

  temp_virtual_adderss_vga = virtualAddr;
  kern_stack_virt = virtualAddr + 0x5000;
}

void setup_kernel_stack(){
  struct kernel_stack *stack = (struct kernel_stack *) page_alloc();
  printf("\nkernel stack address physical:%x", stack);
  map_process(kern_stack_virt, (uint64_t)stack);
  printf("\nkernel Stack virtual address %x \n  mapped to physical address %x", kern_stack_virt, stack);
}

void map_process(uint64_t virtual_address, uint64_t physical_address){

  struct page_directory_t *pdpt;
  struct page_directory_t  *pdt;
  struct page_directory_t   *pt;
  struct page_directory_t *pml4e = (struct page_directory_t *)cur_pml4e_virt;

  uint64_t pml4e_entry =  pml4e->entries[PML4E_INDEX((uint64_t)virtual_address)];
  if(is_present(&pml4e_entry)){
    pdpt = (struct page_directory_t *)get_address(&pml4e_entry);
  }else{
    printf("\nPDPE not Present");
    pdpt = (struct page_directory_t *)page_alloc();
    uint64_t pdpt_entry = (uint64_t)pdpt;
    printf("\nentry %x",pdpt_entry);
    add_attribute(&pdpt_entry,PRESENT);
    add_attribute(&pdpt_entry,WRITABLE);
    add_attribute(&pdpt_entry,USER_MODE);
    pml4e->entries[PML4E_INDEX((uint64_t)virtual_address)] = pdpt_entry;
  }

   uint64_t virtual_pdpt = (uint64_t) pdpt |IDENTITY_MAP_VIRTUAL_START;
   pdpt = (struct page_directory_t*) virtual_pdpt;
  

  uint64_t pdpt_entry =  pdpt->entries[PDPE_INDEX((uint64_t)virtual_address)];

  if(is_present(&pdpt_entry)){
    pdt = (struct page_directory_t *)get_address(&pdpt_entry) ;

  }else{
    pdt = (struct page_directory_t *)page_alloc();
    uint64_t pdt_entry = (uint64_t)pdt;
    add_attribute(&pdt_entry,PRESENT);
    add_attribute(&pdt_entry,WRITABLE);
    add_attribute(&pdt_entry,USER_MODE);
    pdpt->entries[PDPE_INDEX((uint64_t)virtual_address)] = pdt_entry;
  }

   uint64_t virtual_pdt = (uint64_t) pdt |IDENTITY_MAP_VIRTUAL_START;
  pdt = (struct page_directory_t*) virtual_pdt;


  uint64_t pdt_entry =  pdt->entries[PDE_INDEX((uint64_t)virtual_address)];

  if(is_present(&pdt_entry)){
    pt = (struct page_directory_t *)get_address(&pdt_entry);
  }else{

    pt = (struct page_directory_t *)page_alloc();
    uint64_t pt_entry = (uint64_t)pt;
    add_attribute(&pt_entry,PRESENT);
    add_attribute(&pt_entry,WRITABLE);
    add_attribute(&pt_entry,USER_MODE);
    pdt->entries[PDE_INDEX((uint64_t)virtual_address)] = pt_entry;
  }

  uint64_t virtual_pt = (uint64_t) pt |IDENTITY_MAP_VIRTUAL_START;
  pt = (struct page_directory_t*) virtual_pt;

  uint64_t entry = physical_address;
  add_attribute(&entry,PRESENT);
  add_attribute(&entry,WRITABLE);
  add_attribute(&entry,USER_MODE);
  pt->entries[PT_INDEX((uint64_t)virtual_address)] = entry;
}

void map(uint64_t virtual_address, uint64_t physical_address){

    struct page_directory_t *pdpt;
    struct page_directory_t  *pdt;
    struct page_directory_t   *pt;
    page_directory_t* pml4e = (cur_pml4e_virt?cur_pml4e_virt:kern_pml4e_phy);
    uint64_t pml4e_entry =  pml4e->entries[PML4E_INDEX((uint64_t)virtual_address)];
    if(is_present(&pml4e_entry)){
      pdpt = (struct page_directory_t *)get_address(&pml4e_entry);     
    }else{
      printf("\nPDPE not Present");
      pdpt = (struct page_directory_t *)page_alloc();
      uint64_t pdpt_entry = (uint64_t)pdpt;
      printf("\nentry %x",pdpt_entry);
      add_attribute(&pdpt_entry,PRESENT);
      add_attribute(&pdpt_entry,WRITABLE);
      add_attribute(&pdpt_entry,USER_MODE);
      pml4e->entries[PML4E_INDEX((uint64_t)virtual_address)] = pdpt_entry;
    }

    uint64_t pdpt_entry =  pdpt->entries[PDPE_INDEX((uint64_t)virtual_address)]; 

    if(is_present(&pdpt_entry)){
      pdt = (struct page_directory_t *)get_address(&pdpt_entry);

    }else{
      pdt = (struct page_directory_t *)page_alloc();
      uint64_t pdt_entry = (uint64_t)pdt;
      add_attribute(&pdt_entry,PRESENT);
      add_attribute(&pdt_entry,WRITABLE);
      add_attribute(&pdt_entry,USER_MODE);
      pdpt->entries[PDPE_INDEX((uint64_t)virtual_address)] = pdt_entry;
    }
   
    uint64_t pdt_entry =  pdt->entries[PDE_INDEX((uint64_t)virtual_address)];

    if(is_present(&pdt_entry)){
      pt = (struct page_directory_t *)get_address(&pdt_entry);
    }else{
      pt = (struct page_directory_t *)page_alloc();
      uint64_t pt_entry = (uint64_t)pt;
      add_attribute(&pt_entry,PRESENT);
      add_attribute(&pt_entry,WRITABLE);
      add_attribute(&pt_entry,USER_MODE);
      pdt->entries[PDE_INDEX((uint64_t)virtual_address)] = pt_entry;
    }

    uint64_t entry = physical_address;
    add_attribute(&entry,PRESENT);
    add_attribute(&entry,WRITABLE);
    add_attribute(&entry,USER_MODE);
    pt->entries[PT_INDEX((uint64_t)virtual_address)] = entry;
}

void identity_mapping(){

  uint64_t virtualaddr = IDENTITY_MAP_VIRTUAL_START;
  uint64_t physaddr = IDENTITY_MAP_PHYSICAL_START;
  uint64_t max_physical  = get_max_phys();
  
  while(physaddr<=max_physical) {
    map(virtualaddr,physaddr);	
    physaddr = physaddr+0x1000UL;
    virtualaddr = virtualaddr+0x1000UL;
  }
 
  uint64_t vgaVirt = temp_virtual_adderss_vga;
  uint64_t vga_offset = 0; //get_VGA_Vaddress();
  while (vga_offset <= 23 * 80 * 2 * 4) {
    map(vgaVirt + vga_offset, VGA_BUFFER + vga_offset);
    vga_offset += 0x1000;
  }
   mem_map =(uint64_t*)(0xFFFFFFFF80000000UL|(uint64_t)mem_map);
   uint64_t v_add  = (uint64_t)kern_pml4e_phy | IDENTITY_MAP_VIRTUAL_START;
   cur_pml4e_virt = kern_pml4e_virt = (page_directory_t*)v_add;
}
 
void load_cr3(page_directory_t* pml4e)
{
  vga_virt_addr = temp_virtual_adderss_vga;
  uint64_t pml4eaddr =(uint64_t)pml4e;
  __asm__ __volatile__("mov %0, %%cr3":: "b"(pml4eaddr));
}

void load_kernel_cr3() {
  load_cr3(kern_pml4e_phy);
}

uint64_t i_virt_to_phy(uint64_t virt) {
  return virt - IDENTITY_MAP_VIRTUAL_START;
}

uint64_t i_phy_to_virt(uint64_t phy) {
  return phy + IDENTITY_MAP_VIRTUAL_START;
}

void* i_virt_alloc() {
  uint64_t phy_add = (uint64_t)page_alloc();
//  printf("\nkernel stack address physical:%x", stack);
  uint64_t virt_add = i_phy_to_virt(phy_add );
  map_process(virt_add, phy_add);
  memset((void*)virt_add, 0, 512*8);
  return (void*)virt_add;
  //printf("\nAllocated virtual address %x \n  mapped to physical address %x", virtual_address_k, stack);
}


#define MASK (~((1ull<<12) - 1))

uint64_t get_phy_addr(uint64_t addr, uint64_t* pml4e) {
  uint64_t* pdpe = (uint64_t*)(i_phy_to_virt(pml4e[0x1ff & (addr >> 39)] & MASK));
  uint64_t* pde = (uint64_t*)(i_phy_to_virt(pdpe[0x1ff & (addr >> 30)] & MASK));
  uint64_t* pt = (uint64_t*)(i_phy_to_virt(pde[0x1ff & (addr >> 21)] & MASK));

  uint64_t phy_addr = pt[0x1ff & (addr >> 12)];

  return phy_addr & MASK;
}

page_directory_t* clone_page_directory(page_directory_t* tab_src, int level) {
  if (level == 1)  {
    //printf("Returning %x\n", (uint64_t)tab_src);
    uint64_t* newFrame = (uint64_t*)i_virt_alloc();
    memcpy(newFrame, tab_src, 4096);
    return (page_directory_t*)newFrame;
  }
  printf("Level: %x\n", level);

  page_directory_t* tab_new = (page_directory_t*)i_virt_alloc();
  int i = 0;
  for (i = 0; i < 512; i++) {
    if (is_present(&tab_src->entries[i])) {
      page_directory_t* newPage = clone_page_directory((page_directory_t*)i_phy_to_virt(tab_src->entries[i] & MASK), level - 1);
      tab_new->entries[i] = i_virt_to_phy((uint64_t)newPage) | (tab_src->entries[i] & 0XFFF);
    }
  }
  return tab_new;
}
