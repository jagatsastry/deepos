#include<stdio.h>
#include<defs.h>
#include <files.h>
#include <string.h>
#include<sys/tarfs.h>
#include<elf.h>
#include<phy_mem.h>
#include<virt_mem.h>
#include <task.h>
#include <sys/gdt.h>



void print_posix_header( struct posix_header_ustar *p){

  if (DEBUG) printf("Tarfs header\n");
  if (DEBUG) printf("Name: %s, Mode: %s, Uid: %s\n",  p->name, p->mode, p->uid);
  if (DEBUG) printf("Gid: %s, Size: %s, Typeflag: %s\n", 
         p->gid, p->size, p->typeflag);

  //  if (DEBUG) printf("\n Posix header :  %s :%s :%s ",p->name,p->size,p->typeflag);

}

extern uint64_t oct_to_dec(char *oct);
extern char _binary_tarfs_start, _binary_tarfs_end;

struct posix_header_ustar* get_elf_file(char* filename, Exe_Format *exeFormatAddr, uint64_t *elf_start_ptr) {

/*
  char* header =  (char*)(&_binary_tarfs_start);
  char *end =  (char*)(&_binary_tarfs_end);
// while(header < (struct posix_header_ustar*)(&_binary_tarfs_end))
  struct posix_header_ustar * h = (struct posix_header_ustar*)header;
   while(header < end - 512 && strcmp(h->name, filename) != 0) {
    int sz = oct_to_dec(h->size);
    if (DEBUG) printf("Name: %s Size: %d Cur: %p\n", h->name, sz, h);
    if (sz % 512 != 0)
      sz += (512 - sz % 512);
    int jump = (512 + sz);
    header = header + jump;
    h = (struct posix_header_ustar*)header;
   }
*/       uint64_t start = (uint64_t)&_binary_tarfs_start;
       uint64_t end = (uint64_t)&_binary_tarfs_end;
       struct posix_header_ustar *ptr;
       ptr = (struct posix_header_ustar *)start;
       while(start < end) {
          ptr = (struct posix_header_ustar *)start;
          if (DEBUG) printf("\nName %s",ptr->name);
          if (DEBUG) printf("\nSize %s",ptr->size);
          if( !strcmp( ptr->name, filename)){
              start = start + sizeof(struct posix_header_ustar );
              break; 
          } 
          int size = 0, i =0;
          while( ptr->size[i] != '\0'){
             size = size*10 + ( ptr->size[i] - '0');
             i++ ;
          }
          //printf("\n Size before sending %d", size ); 
          size = octal_decimal( size );
          //printf("\nThe size is %d", size); 
          int offset = 0;
          if( size % 512 != 0 )
          {
             offset =   512 - size % 512;
          }
          start = start + 512 + size + offset ;
      }

   if (start >= end) {
    if (DEBUG) printf("ERROR: %s not found in tarfs\n", filename);
    return NULL;
   }
   struct posix_header_ustar *h = (struct posix_header_ustar*) start;
  
   programHeader pdr;
   *elf_start_ptr = (uint64_t) h; 
   int x = Parse_ELF_Executable((char*)h, oct_to_dec((h-1)->size),exeFormatAddr,&pdr);
   if (DEBUG) printf("Status: %d\n", x);

   return h;
}

void map_exe_format(Exe_Format* exeFormatAddr, uint64_t *elf_start_ptr){

  if (DEBUG) printf("\nStart Address %x",exeFormatAddr->entryAddr);
  int i =0, cur_vma_idx = VMA_SEGMENT_START;
  for (; i<exeFormatAddr->numSegments; i++){
    if (DEBUG) printf("I :%d\n", i);
    
    struct Exe_Segment segment  = (struct Exe_Segment)exeFormatAddr->segmentList[i];
    uint64_t start = segment.vaddr;
    uint64_t end = segment.vaddr + segment.sizeInMemory;
   
    
    // if (DEBUG) printf("\n Start: %x End: %x", start ,end);
   
    uint64_t pageNeeded = 0;
    uint64_t least_start = 0;
    uint64_t max_end = 0;

    if(end-start !=0){

      
      least_start = (start/0x1000)*0x1000;
      max_end = (end/0x1000)*0x1000 + 0x1000;

      //   if (DEBUG) printf("\n least_start : %x",least_start);
      // if (DEBUG) printf("\n max_end : %x",max_end);
      pageNeeded = (max_end - least_start)/0x1000;
      // if (DEBUG) printf("\nPage Needed :%x",pageNeeded);
   
    }
    if (DEBUG) printf("Seg: %d least_start: %x, max_end: %x\n", i, least_start, max_end);
    if(pageNeeded !=0){
     current_task->vma[cur_vma_idx].start_addr = least_start;
     current_task->vma[cur_vma_idx].end_addr = max_end;
     cur_vma_idx++;
        while(pageNeeded !=0){

	  void *physicalAddress = page_alloc();
	  //  if (DEBUG) printf("\n Physical Address :%x",physicalAddress);
	 
	  map_process(least_start,(uint64_t)physicalAddress);
	  
	  pageNeeded -= 1;
	  least_start += 0x1000;

       }

	uint64_t ondiskstart = segment.offsetInFile + *elf_start_ptr;

	uint64_t ondiskfinish = segment.offsetInFile + *elf_start_ptr + segment.sizeInMemory;
       	if (DEBUG) printf("\n ondiskstart: %x odiskfinish: %x", ondiskstart,ondiskfinish);
	
	uint64_t size = segment.sizeInMemory;

	char *ondisk =  (char *) ondiskstart;
	char *vadd = (char *) start;
	//	if (DEBUG) printf("\nvadd %x",vadd);
	while(size){

	  *vadd = *ondisk;
	  vadd++;
	  ondisk++;

	  size--;
	}
     
    }
 }
}
