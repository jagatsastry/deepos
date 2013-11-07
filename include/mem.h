#include<defs.h>

void init_phys_mem(uint32_t* modulep, void* kernmem, void* physbase, void* physfree);
uint64_t mm_phy_alloc_page();
uint64_t align_next_page(uint64_t ptr);
