#include<stdio.h>
#include<defs.h>
#include<phy_mem.h>
#define PHYS_MEM_GB 64ull
#define MEM_MAP_SIZE 4096
#define PHYS_PAGE_SIZE 4096
#define PHYS_BLOCK_SIZE 512
#define MEM_NOTAVAILABLE 0

static uint64_t ker_physbase;
static uint64_t ker_physfree;
uint64_t *mem_map;
static uint64_t max_phys;

void init_physbase_physfree(uint64_t physbase, uint64_t physfree){
  ker_physbase = physbase;
  ker_physfree = physfree;
}

void set_physfree(uint64_t physfree){
  ker_physfree = physfree;
}

void set_physbase(uint64_t physbase){
  ker_physbase = physbase;
}

uint64_t get_physfree(){
  return ker_physfree;
}

void set_max_phys(uint64_t max){
  max_phys = max;
}

uint64_t get_max_phys(){
  return max_phys;
}

uint64_t get_physbase(){
  return ker_physbase;
}

void mmap_set (uint64_t bit) {
   mem_map[bit / 64ull] |= (1ull << (bit % 64ull));
}

void mmap_unset (uint64_t bit) {
  mem_map[bit / 64ull] &= ~ (1ull << (bit % 64ull));
}

uint64_t mmap_test (uint64_t bit) {
  return mem_map[bit / 64ull] &  (1ull << (bit % 64ull));
}

void initialize_mem_map() {
 uint64_t physfree = get_physfree();
 mem_map = (uint64_t*) ker_physfree;
 uint64_t  i = 0;
 for(i = 0;i<MEM_MAP_SIZE;i++){
       mem_map[i] = MEM_NOTAVAILABLE;
  }

 physfree += (uint64_t)(MEM_MAP_SIZE*64ull/8);
 set_physfree(physfree);
}

void init_region (uint64_t  base, uint64_t limit) {

  set_max_phys(limit);
  uint64_t start = base / PHYS_PAGE_SIZE;
  uint64_t end = limit / PHYS_PAGE_SIZE;
  mmap_unset(0);
  mmap_unset(1);
  for (; start<=end; start++) {
    if((start*PHYS_PAGE_SIZE) <=  ker_physfree  && (start*PHYS_PAGE_SIZE) >= ker_physbase){
      
    }else{
      mmap_set(start);
    }
   }
 
  uint64_t sta = 0;
  for(uint64_t low=0; low < 0x100000; low += 0x1000){
    sta ++;
    mmap_unset(sta);
  }
}

uint64_t get_index_of_free_page(){
 uint64_t  i =0;
 for( i =0; i<MEM_MAP_SIZE; i++){  

  uint64_t k =0;
  for(k = 0; k<64ull; k++){
    uint64_t  bit = 1ull << k;
     if(mem_map[i]&bit){
         return i*64ull+k;
    }
   }
  }
  return -1;
}

void* page_alloc() {
  uint64_t index =  get_index_of_free_page();
  if (index  == -1) {
    return NULL;//out of memory
    printf("Error: Out of memory!");
    while(1);
  }
  mmap_unset(index);
  uint64_t  addr = index * PHYS_PAGE_SIZE;

  return (void*)addr;
}

void page_dealloc (void* p) {
  uint64_t addr = (uint64_t) p;
  uint64_t index = addr / PHYS_PAGE_SIZE;
  mmap_set (index);
}
