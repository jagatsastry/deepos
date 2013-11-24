/*Adapted from Rishabh Sharma's SBU OS. Distriuted under Lesser GPL license*/
#include<stdio.h>
#include<defs.h>
#include<virt_mem.h>
#include<phy_mem.h>
#include <utilities.h>
#define ADDITIONAL 0x4800
#define IDENTITY_MAP_VIRTUAL_START 0xFFFFFFFFF0000000UL
#define IDENTITY_MAP_PHYSICAL_START 0x0UL
#define VGA_BUFFER 0xB8000 
extern char kernmem;
struct PML4 *pml4;
extern uint64_t *mem_map;
extern uint64_t vga_virt_addr;

uint64_t virtual_address_vga;
uint64_t virtual_address_k;
uint64_t virtual_address_pmlf4;

uint64_t get_VGA_Vaddress(){
  return virtual_address_vga;
}

void set_VGA_Vaddress(uint64_t vga_address){
  virtual_address_vga = vga_address;
}

uint64_t get_PMLF4_Vaddress(){
  return virtual_address_pmlf4;
}

void set_PMLF4_Vaddress(uint64_t pmlf4_address){
  virtual_address_pmlf4 = pmlf4_address;
}

void map_kernel(){
  uint64_t physbase = get_physbase();
  uint64_t physfree = get_physfree();
  pml4 = (struct PML4 *) page_alloc();

  struct PDPT *pdpt = (struct PDPT *) page_alloc();
  
  uint64_t pdpt_entry = (uint64_t)pdpt;
  add_attribute(&pdpt_entry,PRESENT);
  add_attribute(&pdpt_entry,WRITABLE);
  add_attribute(&pdpt_entry,USER_MODE);
  pml4->entries[PML4_INDEX((uint64_t)&kernmem)] = pdpt_entry;
  struct PDT *pdt = (struct PDT *) page_alloc();
  uint64_t pdt_entry = (uint64_t)pdt;
  add_attribute(&pdt_entry,PRESENT);
  add_attribute(&pdt_entry,WRITABLE);
  add_attribute(&pdt_entry,USER_MODE);
  pdpt->entries[PDPT_INDEX((uint64_t)&kernmem)] = pdt_entry;
  struct PT *pt = (struct PT *) page_alloc();
  uint64_t pt_entry = (uint64_t)pt;
  add_attribute(&pt_entry,PRESENT);
  add_attribute(&pt_entry,WRITABLE);
  add_attribute(&pt_entry,USER_MODE);
  pdt->entries[PDT_INDEX((uint64_t)&kernmem)] = pt_entry;
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

  set_VGA_Vaddress(virtualAddr);
  virtual_address_k = virtualAddr + 0x5000;
}

void setup_kernel_stack(){
  struct kernel_stack *stack = (struct kernel_stack *) page_alloc();
  printf("\nkernel stack address physical:%x", stack);
  map_process(virtual_address_k, (uint64_t)stack);
  printf("\nkernel Stack virtual address %x \n  mapped to physical address %x", virtual_address_k, stack);
}

void map_process(uint64_t virtual_address, uint64_t physical_address){

  struct PDPT *pdpt;
  struct PDT  *pdt;
  struct PT   *pt;
  struct PML4 *pml4 = (struct PML4 *)get_PMLF4_Vaddress();

  uint64_t pml4_entry =  pml4->entries[PML4_INDEX((uint64_t)virtual_address)];
  if(is_present(&pml4_entry)){
    pdpt = (struct PDPT *)get_address(&pml4_entry);
  }else{
    printf("\nPDPT not Present");
    pdpt = (struct PDPT *)page_alloc();
    uint64_t pdpt_entry = (uint64_t)pdpt;
    printf("\nentry %x",pdpt_entry);
    add_attribute(&pdpt_entry,PRESENT);
    add_attribute(&pdpt_entry,WRITABLE);
    add_attribute(&pdpt_entry,USER_MODE);
    pml4->entries[PML4_INDEX((uint64_t)virtual_address)] = pdpt_entry;
  }

   uint64_t virtual_pdpt = (uint64_t) pdpt |IDENTITY_MAP_VIRTUAL_START;
   pdpt = (struct PDPT*) virtual_pdpt;
  

  uint64_t pdpt_entry =  pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)];

  if(is_present(&pdpt_entry)){
    pdt = (struct PDT *)get_address(&pdpt_entry) ;

  }else{
    pdt = (struct PDT *)page_alloc();
    uint64_t pdt_entry = (uint64_t)pdt;
    add_attribute(&pdt_entry,PRESENT);
    add_attribute(&pdt_entry,WRITABLE);
    add_attribute(&pdt_entry,USER_MODE);
    pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)] = pdt_entry;
  }

   uint64_t virtual_pdt = (uint64_t) pdt |IDENTITY_MAP_VIRTUAL_START;
  pdt = (struct PDT*) virtual_pdt;


  uint64_t pdt_entry =  pdt->entries[PDT_INDEX((uint64_t)virtual_address)];

  if(is_present(&pdt_entry)){
    pt = (struct PT *)get_address(&pdt_entry);
  }else{

    pt = (struct PT *)page_alloc();
    uint64_t pt_entry = (uint64_t)pt;
    add_attribute(&pt_entry,PRESENT);
    add_attribute(&pt_entry,WRITABLE);
    add_attribute(&pt_entry,USER_MODE);
    pdt->entries[PDT_INDEX((uint64_t)virtual_address)] = pt_entry;
  }

  uint64_t virtual_pt = (uint64_t) pt |IDENTITY_MAP_VIRTUAL_START;
  pt = (struct PT*) virtual_pt;

  uint64_t entry = physical_address;
  add_attribute(&entry,PRESENT);
  add_attribute(&entry,WRITABLE);
  add_attribute(&entry,USER_MODE);
  pt->entries[PT_INDEX((uint64_t)virtual_address)] = entry;
}

void map(uint64_t virtual_address, uint64_t physical_address){

    struct PDPT *pdpt;
    struct PDT  *pdt;
    struct PT   *pt;

    uint64_t pml4_entry =  pml4->entries[PML4_INDEX((uint64_t)virtual_address)];
    if(is_present(&pml4_entry)){
      pdpt = (struct PDPT *)get_address(&pml4_entry);     
    }else{
      printf("\nPDPT not Present");
      pdpt = (struct PDPT *)page_alloc();
      uint64_t pdpt_entry = (uint64_t)pdpt;
      printf("\nentry %x",pdpt_entry);
      add_attribute(&pdpt_entry,PRESENT);
      add_attribute(&pdpt_entry,WRITABLE);
      add_attribute(&pdpt_entry,USER_MODE);
      pml4->entries[PML4_INDEX((uint64_t)virtual_address)] = pdpt_entry;
    }

    uint64_t pdpt_entry =  pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)]; 

    if(is_present(&pdpt_entry)){
      pdt = (struct PDT *)get_address(&pdpt_entry);

    }else{
      pdt = (struct PDT *)page_alloc();
      uint64_t pdt_entry = (uint64_t)pdt;
      add_attribute(&pdt_entry,PRESENT);
      add_attribute(&pdt_entry,WRITABLE);
      add_attribute(&pdt_entry,USER_MODE);
      pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)] = pdt_entry;
    }
   
    uint64_t pdt_entry =  pdt->entries[PDT_INDEX((uint64_t)virtual_address)];

    if(is_present(&pdt_entry)){
      pt = (struct PT *)get_address(&pdt_entry);
    }else{
      pt = (struct PT *)page_alloc();
      uint64_t pt_entry = (uint64_t)pt;
      add_attribute(&pt_entry,PRESENT);
      add_attribute(&pt_entry,WRITABLE);
      add_attribute(&pt_entry,USER_MODE);
      pdt->entries[PDT_INDEX((uint64_t)virtual_address)] = pt_entry;
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
 
 uint64_t vgaVirt = get_VGA_Vaddress();
  uint64_t vga_offset = 0; //get_VGA_Vaddress();
  while (vga_offset <= 23 * 80 * 2 * 4) {
    map(vgaVirt + vga_offset, VGA_BUFFER + vga_offset);
    vga_offset += 0x1000;
  }
   mem_map =(uint64_t*)(0xFFFFFFFF80000000UL|(uint64_t)mem_map);
   uint64_t v_add  = (uint64_t)pml4 | IDENTITY_MAP_VIRTUAL_START;
   set_PMLF4_Vaddress(v_add);
}
 
void load_cr3()
{
  vga_virt_addr = get_VGA_Vaddress() ;
  uint64_t pml4addr =(uint64_t)pml4;
  __asm__ __volatile__("mov %0, %%cr3":: "b"(pml4addr));
}

