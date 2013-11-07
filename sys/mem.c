#include <addr.h>
#include <stdlib.h>
#include <defs.h>
#include <stdio.h>
#include <pt.h>

#define MAX_RAM_SIZE 256 * 1024 * 1024 //2^64

#define PAGES_PER_GROUP 64

#define NUM_PAGES MAX_RAM_SIZE/PAGE_SIZE

#define PHY_BITMAP_SIZE  MAX_RAM_SIZE/(PAGE_SIZE * 64)

static uint64_t *phy_bitmap = NULL; //[MAX_RAM_SIZE/(PAGE_SIZE * 64)];

uint64_t kernmem_offset, kern_physbase, kern_physfree;

void set_page_used(uint64_t pageNum) {
    phy_bitmap[pageNum/64] |= (1ULL<<(pageNum%64));
}

void set_page_free(uint64_t pageNum) {
    phy_bitmap[pageNum/64] &= ~(1ULL<<(pageNum%64));
}

void set_page_used_for_addr(uint64_t ptr) {
    set_page_used(ptr/PAGE_SIZE);
}

void set_page_free_for_addr(uint64_t ptr) {
    set_page_free(ptr/PAGE_SIZE);
}

uint64_t align_next_page(uint64_t ptr) {
    return (ptr/PAGE_SIZE) << 12; //HARDCODED;
}

void init_phys_mem(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {

    printf("Initializing physical memory\n");
    phy_bitmap = (uint64_t*)physfree;
    physfree += PHY_BITMAP_SIZE;
    
    kern_physfree = (uint64_t)physfree;
    kern_physbase = (uint64_t)physbase;
    kernmem_offset = (uint64_t)kernmem;

    int i = 0;
    //Mark all as occupied
    memset(phy_bitmap, 0xFF, sizeof(phy_bitmap));

    struct smap_t {
        uint64_t base, length;
        uint32_t type;
    }__attribute__((packed)) *smap;

    while(modulep[0] != 0x9001) modulep += modulep[1]+2;
    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
        if (smap->type == 1 /* memory */ && smap->length != 0) {
            uint64_t base = align_next_page(smap->base);
            uint64_t length = base == smap->base ? smap->length: (smap->length - (base - smap->base));
            for (i =0; i < length; i+=PAGE_SIZE)
                set_page_free(base);
        }
    }
    char *pb = (char*)physbase;
    for (; pb < (char*)physfree; pb+=PAGE_SIZE) {
        set_page_used_for_addr((uint64_t)pb);
    }
    
    char* vid = (char*)vga_phy_addr;
    
    for (i = 0; i < 80*25*4; i+= PAGE_SIZE)
        set_page_used_for_addr((uint64_t)(vid + i));

    set_page_used((uint64_t)(0));

    for (i = 0; i < 0x200000; i+= PAGE_SIZE)
        set_page_used_for_addr((uint64_t)(i));
    
    printf("Initializing page tables\n");
   pt_initialise(modulep);
    printf("Initialized page tables\n");
}

/****************************************************
 * mm_phy_free_page: free given page.
 */
void mm_phy_free_page(uint64_t page) {

  if(!page) 
    return;

  set_page_free(page);
}


//Returns a specific page frame number, not the address.
uint64_t mm_phy_alloc_page() {

  uint64_t g=0;
  for ( g=0; g< sizeof phy_bitmap / sizeof phy_bitmap[0]; ++g) {
    uint64_t _g = phy_bitmap[g];
    if (_g != 0xffffffffffffffff) {
      uint8_t p = 0;
      for (p = 0; p< 64; ++p) {
        if ((_g & (1 << p)) == 0) {

          phy_bitmap[g] |= (1 << p);
          if (((g * 64) + p) == 0)
            printf("ERR: ALLOCATING PAGE ZERO!!!\n");

            return (g * PAGES_PER_GROUP) + p;
        }
      }
    }
  }
  printf("ERR: No free pages\n");
  return 0; /* no free pages  */
}

