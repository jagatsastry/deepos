#include <stdlib.h>
#include <defs.h>
#include <stdio.h>

#define MAX_RAM_SIZE 1<<64 //2^64

#define PAGE_SIZE 4096

#define NUM_PAGES MAX_RAM_SIZE/PAGE_SIZE

static uint64_t phy_bitmap[MAX_RAM_SIZE/(PAGE_SIZE * 64)];

void mark_page_used(uint64_t ptr) {
    uint64_t pageNum = ptr/PAGE_SIZE;
    phy_bitmap[pageNum/64)] |= (1<<(pageNum%64));
}

void mark_page_free(uint64_t ptr) {
    uint64_t pageNum = ptr/PAGE_SIZE;
    phy_bitmap[pageNum/64)] &= ~(1<<(pageNum%64));
}

uint64_t align_next_page(uint64_t ptr) {
    return (ptr/PAGE_SIZE) << 12; //HARDCODED;
}

void init_phys_mem(uint32_t* modulep, void* physbase, void* physfree) {

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
                mark_page_free(base);
        }
    }
    char *pb = (char*)physbase;
    for (; pb < (char*)physfree; pb+=PAGE_SIZE) {
        mark_page_used((uint64_t)pb);
    }
    
    char* vid = (char*)0xB8000;
    
    for (i = 0; i < 80*25*4; i+= PAGE_SIZE)
        mark_page_used((uint64_t)(vid + i));
}

void init_pt(
