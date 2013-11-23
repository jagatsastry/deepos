#include<stdio.h>
#include<idt.h>
#include<utilities.h>
#include<pic.h>
#include <phy_mem.h>
#include <virt_mem.h>

void init_mem_mgmt(uint32_t* modulep, void* kernmem, void* physbase, void* physfree);

void init_kernel(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {
    init_pics();
    idtStart();
    printf("xx %x %x \n", (uint64_t)physbase, (uint64_t)physfree);
    init_mem_mgmt(modulep, kernmem, physbase, physfree);
    printf("Booting Deep-OS");
}

void init_mem_mgmt(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {
  printf("Initializing memory management\n");
  init_physbase_physfree((uint64_t)physbase,(uint64_t)physfree);
  initialize_mem_map();

  struct smap_t *smap;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
      init_region (smap->base,smap->base + smap->length);

    }
  }
  map_kernel();
  identity_mapping();
  load_cr3();
  printf("Finished initializing mem mgmt\n");
}

