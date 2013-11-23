/*Adapted from Rishabh Sharma's SBU OS. Distriuted under Lesser GPL license*/

#ifndef _PHYMEMMAN_H
#define _PHYMEMMAN_H

#include<defs.h>

void initialize_mem_map();
void init_physbase_physfree(uint64_t, uint64_t);
void set_physfree(uint64_t);
void set_physbase(uint64_t);
uint64_t get_physfree();
uint64_t get_physbase();
void mmap_set (uint64_t);
void mmap_unset (uint64_t);
uint64_t  get_index_of_free_page();
void init_region (uint64_t , uint64_t);
void* page_alloc();
void page_dealloc (void* ); 
void set_max_phys(uint64_t);
uint64_t get_max_phys();

#endif
