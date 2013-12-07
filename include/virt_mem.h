#ifndef _VIRMEMMAN_H
#define _VIRMEMMAN_H

#include<defs.h>
#include <task.h>

#define PRESENT 0x1        
#define WRITABLE 0x2       
#define USER_MODE 0x4       
#define PWT 0x8      
#define PCD 0x10     
#define ACCESED 0x20      
#define DIRTY 0x40      
#define PAT 0x80     
#define GLOBAL 0x100      
#define NX 0x8000000000000 
#define FRAME 0xFFFFFFFFFFFFF000

#define MEM_FLAG_MASK (~((1ull<<12) - 1))

typedef struct page_directory_t{
  uint64_t entries[512];
} page_directory_t;


void add_attribute(uint64_t*, uint64_t);
void delete_attribute(uint64_t*, uint64_t);
void set_address(uint64_t*, uint64_t);
uint64_t get_address(uint64_t*);
int is_present(uint64_t*);
int is_writable(uint64_t*);
int is_user(uint64_t*);

void map(uint64_t, uint64_t);
void map_kernel();
void load_cr3();
void load_kernel_stack();
void setup_kernel_stack();
void map_process(uint64_t , uint64_t);
struct task;
void map_process_specific(uint64_t, uint64_t, page_directory_t *, struct task*);


#define PML4E_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPE_INDEX(x) (((x) >> 30) & 0x1FF)
#define PDE_INDEX(x) (((x) >> 21) & 0x1FF)
#define PT_INDEX(x) (((x) >> 12) & 0x1FF)
//#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

//uint64_t pml4_lookup(struct PML4E *, uint64_t);
//uint64_t pdpt_lookup(struct PDPE *, uint64_t);
//uint64_t pdt_lookup(struct PDE *, uint64_t);
//uint64_t pt_lookup(struct PT *, uint64_t);


//void map(uint64_t, uint64_t);
void identity_mapping();

void* i_virt_alloc();
uint64_t i_virt_to_phy(uint64_t virt);
uint64_t i_phy_to_virt(uint64_t phy) ;
page_directory_t* clone_page_directory(page_directory_t* tab_src, int level);  
void *kmalloc(size_t size);
void* brk();

#endif
