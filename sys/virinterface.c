#include<stdio.h>
#include<defs.h>
#include<virt_mem.h>

void add_attribute(uint64_t* entry, uint64_t attribute){

  *entry |= attribute; 
}

void delete_attribute(uint64_t* entry, uint64_t attribute){

  *entry &= ~attribute;
}

void set_address(uint64_t* entry, uint64_t address){

  *entry = (*entry & ~FRAME) | address;

}

uint64_t get_address(uint64_t* entry){

  return (*entry & FRAME);

}

int is_present(uint64_t* entry){

  return (*entry & PRESENT);

}

int is_writable(uint64_t* entry){

  return (*entry & WRITABLE);

}

int is_user(uint64_t* entry){

  return (*entry & USER_MODE);

}

uint64_t* pml4_lookup(struct PML4 *pml4, uint64_t virtual_address){

  return &pml4->entries[PML4_INDEX(virtual_address)];

}

uint64_t*  pdpt_lookup(struct PDPT *pdpt, uint64_t virtual_address){

  return &pdpt->entries[PDPT_INDEX(virtual_address)];

}

uint64_t* pdt_lookup(struct PDT *pdt, uint64_t virtual_address){

  return &pdt->entries[PDT_INDEX(virtual_address)];

}

uint64_t* pt_lookup(struct PT *pt, uint64_t virtual_address){

  return &pt->entries[PT_INDEX(virtual_address)];

}

