#include<stdio.h>
#include<defs.h>
#include<task.h>
#include<stdlib.h>
#include<virt_mem.h>
#include<phy_mem.h>
#include<utilities.h>
#define PHYSBASE_OFFSET 0x4800
#define DUMB_MAPPING_VIRT 0xFFFFFFFFF0000000UL
#define DUMB_MAPPING_PHYS 0x0UL
#define VGA_BUFFER 0xB8000 
extern char kernmem;
extern uint64_t *mem_map;
extern uint64_t vga_virt_addr;

uint64_t temp_virtual_adderss_vga;
uint64_t kern_stack_virt;

struct page_directory_t *kern_pml4e_phy;
struct page_directory_t *kern_pml4e_virt;

struct page_directory_t *cur_pml4e_virt; 
 
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
  return virt - DUMB_MAPPING_VIRT;
}

uint64_t i_phy_to_virt(uint64_t phy) {
  return phy + DUMB_MAPPING_VIRT;
}

void* i_virt_alloc() {
  uint64_t phy_add = (uint64_t)page_alloc(); 
  uint64_t virt_add = i_phy_to_virt(phy_add );
  map_process(virt_add, phy_add);
  memset((void*)virt_add, 0, 4096);
  phy_add = (uint64_t)page_alloc();
  virt_add = i_phy_to_virt(phy_add );
  map_process(virt_add, phy_add);
  memset((void*)virt_add, 0, 4096);

  return (void*)virt_add;
}

uint64_t get_phy_addr(uint64_t addr, page_directory_t* pml4e) {
  uint64_t* pdpe = (uint64_t*)(i_phy_to_virt(pml4e->entries[0x1ff & (addr >> 39)] & MEM_FLAG_MASK));
  uint64_t* pde = (uint64_t*)(i_phy_to_virt(pdpe[0x1ff & (addr >> 30)] & MEM_FLAG_MASK));
  uint64_t* pt = (uint64_t*)(i_phy_to_virt(pde[0x1ff & (addr >> 21)] & MEM_FLAG_MASK));

  uint64_t phy_addr = pt[0x1ff & (addr >> 12)];

  return phy_addr & MEM_FLAG_MASK;
}

#define ENABLE_COW_MASK 0x100
page_directory_t* clone_page_directory_old(page_directory_t* tab_src, int level) {
  if(i_virt_to_phy((uint64_t)tab_src) == 0x208000) {
    if (DEBUG) printf("Alert: Wanted stuff");
  }

  if (level == 0)  {
    if (i_virt_to_phy((uint64_t)tab_src) < 0x100000) return tab_src;
    add_attribute((uint64_t*)tab_src, ENABLE_COW_MASK);
    delete_attribute((uint64_t*)tab_src, WRITABLE);
    return tab_src;
  }

  page_directory_t* tab_new = (page_directory_t*)i_virt_alloc();
  int i = 0;
  for (i = 0; i < 512; i++) {
    if (is_present(&tab_src->entries[i])) {
      uint64_t virt = i_phy_to_virt(tab_src->entries[i] & MEM_FLAG_MASK);
      page_directory_t* newPage = clone_page_directory_old((page_directory_t*)virt, level - 1);
      tab_new->entries[i] = i_virt_to_phy((uint64_t)newPage) | (tab_src->entries[i] & 0XFFF);
    }
  }
  return tab_new;
}

void add_to_vma(char *start, char *end) {

   for (int i = 0; i <10; i++) {
     if (current_task->vma[i].start_addr == NULL) {
      current_task->vma[i].start_addr = (uint64_t)start;
      current_task->vma[i].end_addr = (uint64_t)end;
      if (DEBUG) printf("Added %x-%x to VMA for proc %d\n", start, end, current_task->id);
      break;
     }
  }
}

void* kmalloc(size_tt size) {
  char *cur_ptr = current_task->cur_ptr;
  char *heap_end = current_task->heap_end;
  if (DEBUG) printf("Val of cur_ptr: %x addr: %x\n", (uint64_t)cur_ptr, (uint64_t)&cur_ptr);
  if (DEBUG) printf("Val of heap_end: %x addr: %x\n", heap_end, &heap_end);
  if (cur_ptr == NULL || cur_ptr + size > heap_end) {
    cur_ptr = (char*)brk();
    heap_end = (char*)cur_ptr + 4095;
    add_to_vma(cur_ptr, heap_end);
  }
  if (cur_ptr == NULL) {
    printf("Err: Out of memory\n");
    return NULL;
  }
  char *temp = cur_ptr;
  current_task->cur_ptr = cur_ptr;
  current_task->heap_end = heap_end;
  current_task->cur_ptr =  current_task->cur_ptr + size;
  return (void*)temp;
}


void* brk() {
  if (current_task->current_mem_usage + 4096 > current_task->mem_limit) {
    printf("Err: Memory limit exceeded for PID %d\n", current_task->id);
    return NULL;
  }
  uint64_t addr = (uint64_t)i_virt_alloc();
  int i = 0;
  for (; i < 10; i++) {
    if (current_task->vma[i].start_addr == NULL) {
      current_task->vma[i].start_addr = addr;
      current_task->vma[i].end_addr = addr + 4095;
      if (DEBUG) printf("PID: %d called sbrk. Inserting %x-%x into vma slot %d\n",
        current_task->id, current_task->vma[i].start_addr,
        current_task->vma[i].start_addr, i);
      break;
    }
  }

  current_task->current_mem_usage += 4096;
  return (void*)addr;
}


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
  physfree = PHYSBASE_OFFSET + physfree;
 
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
  map_process(kern_stack_virt, (uint64_t)stack);
}

void map_process(uint64_t virtual_address, uint64_t physical_address){
  map_process_specific(virtual_address, physical_address, cur_pml4e_virt, (task_t*)current_task);
}

void map_process_specific(uint64_t virtual_address, uint64_t physical_address, page_directory_t * pml4e, task_t *task){
  if(task)
    task->current_mem_usage += 4096;

  struct page_directory_t *pdpt;
  struct page_directory_t  *pdt;
  struct page_directory_t   *pt;

  uint64_t pml4e_entry =  pml4e->entries[PML4E_INDEX((uint64_t)virtual_address)];
  if(is_present(&pml4e_entry)){
    pdpt = (struct page_directory_t *)get_address(&pml4e_entry);
  }else{
    pdpt = (struct page_directory_t *)page_alloc();
    uint64_t pdpt_entry = (uint64_t)pdpt;
    add_attribute(&pdpt_entry,PRESENT);
    add_attribute(&pdpt_entry,WRITABLE);
    add_attribute(&pdpt_entry,USER_MODE);
    pml4e->entries[PML4E_INDEX((uint64_t)virtual_address)] = pdpt_entry;
  }

   uint64_t virtual_pdpt = (uint64_t) pdpt |DUMB_MAPPING_VIRT;
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

   uint64_t virtual_pdt = (uint64_t) pdt |DUMB_MAPPING_VIRT;
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

  uint64_t virtual_pt = (uint64_t) pt |DUMB_MAPPING_VIRT;
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
      if (DEBUG) printf("\nPDPE not Present");
      pdpt = (struct page_directory_t *)page_alloc();
      uint64_t pdpt_entry = (uint64_t)pdpt;
      if (DEBUG) printf("\nentry %x",pdpt_entry);
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

  uint64_t virtualaddr = DUMB_MAPPING_VIRT;
  uint64_t physaddr = DUMB_MAPPING_PHYS;
  uint64_t max_physical  = get_max_phys();
  
  while(physaddr<=max_physical) {
    map(virtualaddr,physaddr);	
    physaddr = physaddr+0x1000UL;
    virtualaddr = virtualaddr+0x1000UL;
  }
 
  uint64_t vgaVirt = temp_virtual_adderss_vga;
  uint64_t vga_offset = 0;
  while (vga_offset <= 23 * 80 * 2 * 4) {
    map(vgaVirt + vga_offset, VGA_BUFFER + vga_offset);
    vga_offset += 0x1000;
  }
   mem_map =(uint64_t*)(0xFFFFFFFF80000000UL|(uint64_t)mem_map);
   uint64_t v_add  = (uint64_t)kern_pml4e_phy | DUMB_MAPPING_VIRT;
   cur_pml4e_virt = kern_pml4e_virt = (page_directory_t*)v_add;
}
