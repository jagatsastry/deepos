/*Adapted from Rishabh Sharma's SBU OS. Distriuted under Lesser GPL license*/
#include<stdio.h>
#include<defs.h>
#include<virt_mem.h>
#include<phy_mem.h>
#define ADDITIONAL 0x4800
#define IDENTITY_MAP_VIRTUAL_START 0xFFFFFFFFF0000000UL
#define IDENTITY_MAP_PHYSICAL_START 0x0UL
#define VGA_BUFFER 0xB8000 
extern char kernmem;
struct PML4 *pml4;
extern uint64_t *mem_map;
extern char *vga_virt_addr;

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


//extern static uint64_t *mem_map; 
void map_kernel(){

  uint64_t physbase = get_physbase();
  uint64_t physfree = get_physfree();
  pml4 = (struct PML4 *) page_alloc();
 
  //  printf("\n pmlf4 :%x",pml4->entries[23]);

  struct PDPT *pdpt = (struct PDPT *) page_alloc();
  
  uint64_t pdpt_entry = (uint64_t)pdpt;
  //  printf("\nentry %x",pdpt_entry);
  add_attribute(&pdpt_entry,PRESENT);
  add_attribute(&pdpt_entry,WRITABLE);
  add_attribute(&pdpt_entry,USER_MODE);
  // printf("\nkernmem:%x",&kernmem);
  //  printf("\nentry :%x",pdpt_entry);
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
  //  printf("\n PLM index:%d",PML4_INDEX((uint64_t)&kernmem));
  // printf("\n PDPT index:%d",PDPT_INDEX((uint64_t)&kernmem));
  // printf("\n PDT index:%d",PDT_INDEX((uint64_t)&kernmem));
  // printf("\n PT index:%d",PDT_INDEX((uint64_t)&kernmem));
   uint64_t virtualAddr = (uint64_t)&kernmem;
   // uint64_t additional = 0x4800;
    physfree = ADDITIONAL + physfree;
    //    printf("\n Physfree: %x",physfree);
 
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

   
   
   // VGA BUFFER

   set_VGA_Vaddress(virtualAddr);

   
   virtual_address_k = virtualAddr + 0x5000;

   /*
   starting_address_of_VGA_buffer = (char *) virtualAddr ;
   uint64_t entry = VGA_BUFFER;
   add_attribute(&entry,PRESENT);
   add_attribute(&entry,WRITABLE);
   add_attribute(&entry,USER_MODE);
   pt->entries[PT_INDEX(virtualAddr)] = entry;
   */

   // virtualAddr+=0x1000;
   
   // FREE LIST
   // mem_map =(uint64_t*)(0xFFFFFFFF80000000UL|(uint64_t)mem_map);
   
   // FREE LIST
     /*
   mem_map = (uint64_t *) virtualAddr;
   uint64_t entry1 = physfree;
   add_attribute(&entry1,PRESENT);
   add_attribute(&entry1,WRITABLE);
   add_attribute(&entry1,USER_MODE);
   pt->entries[PT_INDEX(virtualAddr)] = entry1;
     */
   // bitMap =(char*)(0xFFFFFFFF80000000UL|(uint64_t)bitMap);  

   // printf("\n pmlf4 : %x : %x",pml4->entries[511],(uint64_t)pdpt);   
   // printf("\n pdpt : %x : %x ",pdpt->entries[510],(uint64_t) pdt);
   // printf("\n pdp : %x : %x ",pdt->entries[1],(uint64_t) pt);
   // printf("\nentry  %x", pt->entries[0]);
  

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

  
  // printf("\n pml4 physical Address :%x", pml4);
  // printf("\n pml4 virtual Address :%x ", get_PMLF4_Vaddress());
  struct PML4 *pml4 = (struct PML4 *)get_PMLF4_Vaddress();

  uint64_t pml4_entry =  pml4->entries[PML4_INDEX((uint64_t)virtual_address)];
  //   printf("\n pmlf_entry :%x\n",pml4_entry);
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
  
   //  printf("\n pdpt  virtual Address :%x ", virtual_pdpt); 

  uint64_t pdpt_entry =  pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)];

  if(is_present(&pdpt_entry)){
    pdt = (struct PDT *)get_address(&pdpt_entry) ;

  }else{
    // printf("\n PDT not present");
    pdt = (struct PDT *)page_alloc();
    uint64_t pdt_entry = (uint64_t)pdt;
    add_attribute(&pdt_entry,PRESENT);
    add_attribute(&pdt_entry,WRITABLE);
    add_attribute(&pdt_entry,USER_MODE);
    pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)] = pdt_entry;
  }

   uint64_t virtual_pdt = (uint64_t) pdt |IDENTITY_MAP_VIRTUAL_START;
  pdt = (struct PDT*) virtual_pdt;

  // printf("\n pdt  virtual Address :%x ", virtual_pdt);


  //    printf("\nBase Address of pdt: %x",pdt);

  uint64_t pdt_entry =  pdt->entries[PDT_INDEX((uint64_t)virtual_address)];
  //printf("\n Index %x",PDT_INDEX((uint64_t)virtual_address));
  // printf("\n pdt_entry %x",pdt_entry);

  if(is_present(&pdt_entry)){
    pt = (struct PT *)get_address(&pdt_entry);
  }else{

    // printf("\nPT not present");
    pt = (struct PT *)page_alloc();
    uint64_t pt_entry = (uint64_t)pt;
    add_attribute(&pt_entry,PRESENT);
    add_attribute(&pt_entry,WRITABLE);
    add_attribute(&pt_entry,USER_MODE);
    pdt->entries[PDT_INDEX((uint64_t)virtual_address)] = pt_entry;
  }

  uint64_t virtual_pt = (uint64_t) pt |IDENTITY_MAP_VIRTUAL_START;
  pt = (struct PT*) virtual_pt;

  //printf("\n pt  virtual Address :%x ", virtual_pt);



  // printf("\nBase Address of pdt: %x",pt);

  // Just Mapping and not allocating

  uint64_t entry = physical_address;
  add_attribute(&entry,PRESENT);
  add_attribute(&entry,WRITABLE);
  add_attribute(&entry,USER_MODE);
  //    printf("\n entry : %x ",entry);
  // pt->entries[PT_INDEX(virtualAddr)] = entry;
  pt->entries[PT_INDEX((uint64_t)virtual_address)] = entry;

  //  printf("\nentry of vitual address  %x at index %d is %x ",virtual_address,PT_INDEX((uint64_t)virtual_address),pt->entries[PT_INDEX((uint64_t)virtual_address)] );

}

void map(uint64_t virtual_address, uint64_t physical_address){

    struct PDPT *pdpt;
    struct PDT  *pdt;
    struct PT   *pt;

    uint64_t pml4_entry =  pml4->entries[PML4_INDEX((uint64_t)virtual_address)];
    //    printf("\n pmlf_entry :%x",pml4_entry);
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

    // printf("\nBase Address of pdpt: %x",pdpt);
    
    //printf("\nEntruuuuu:  %d",PDPT_INDEX((uint64_t)virtual_address));
    uint64_t pdpt_entry =  pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)]; 

    if(is_present(&pdpt_entry)){
      pdt = (struct PDT *)get_address(&pdpt_entry);

    }else{
      // printf("\n PDT not present");
      pdt = (struct PDT *)page_alloc();
      uint64_t pdt_entry = (uint64_t)pdt;
      add_attribute(&pdt_entry,PRESENT);
      add_attribute(&pdt_entry,WRITABLE);
      add_attribute(&pdt_entry,USER_MODE);
      pdpt->entries[PDPT_INDEX((uint64_t)virtual_address)] = pdt_entry;
    }
   
    //    printf("\nBase Address of pdt: %x",pdt);

    uint64_t pdt_entry =  pdt->entries[PDT_INDEX((uint64_t)virtual_address)];
    //printf("\n Index %x",PDT_INDEX((uint64_t)virtual_address));
    // printf("\n pdt_entry %x",pdt_entry);

    if(is_present(&pdt_entry)){
      pt = (struct PT *)get_address(&pdt_entry);
    }else{
      
      // printf("\nPT not present");
      pt = (struct PT *)page_alloc();
      uint64_t pt_entry = (uint64_t)pt;
      add_attribute(&pt_entry,PRESENT);
      add_attribute(&pt_entry,WRITABLE);
      add_attribute(&pt_entry,USER_MODE);
      pdt->entries[PDT_INDEX((uint64_t)virtual_address)] = pt_entry;
    }

    // printf("\nBase Address of pdt: %x",pt);
    
    // Just Mapping and not allocating 

    uint64_t entry = physical_address;
    add_attribute(&entry,PRESENT);
    add_attribute(&entry,WRITABLE);
    add_attribute(&entry,USER_MODE);
    //    printf("\n entry : %x ",entry);
    // pt->entries[PT_INDEX(virtualAddr)] = entry;
    // printf("\n Index : %d", PT_INDEX((uint64_t)virtual_address));
    pt->entries[PT_INDEX((uint64_t)virtual_address)] = entry;
    //    printf("\nentry of vitual address  %x at index %d is %x ",virtual_address,PT_INDEX((uint64_t)virtual_address),pt->entries[PT_INDEX((uint64_t)virtual_address)] );  
    
    
    /*
    struct PT *pt = (struct PT *) page_alloc();
    uint64_t pt_entry = (uint64_t)pt;
    add_attribute(&pt_entry,PRESENT);
    add_attribute(&pt_entry,WRITABLE);
    add_attribute(&pt_entry,USER_MODE);
    pdt->entries[PDT_INDEX((uint64_t)&kernmem)] = pt_entry;
    */
    

    // if(pml4_entry


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
 
     
     map(get_VGA_Vaddress(),VGA_BUFFER);
     vga_virt_addr = (char *) get_VGA_Vaddress() ;

     mem_map =(uint64_t*)(0xFFFFFFFF80000000UL|(uint64_t)mem_map);

     uint64_t v_add  = (uint64_t)pml4 | IDENTITY_MAP_VIRTUAL_START;

     set_PMLF4_Vaddress(v_add);
    
 }
 

void load_cr3()
{
  uint64_t pml4addr =(uint64_t)pml4;
  __asm__ __volatile__("mov %0, %%cr3":: "b"(pml4addr));
  /*
__asm__ __volatile__( "inb  %1, %0;"
		      :"=a" (ret)
		      :"d"  (port)   );
  */
}
