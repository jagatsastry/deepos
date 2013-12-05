#include<stdio.h>
#include<defs.h>
#include<sys/tarfs.h>
#include<elf.h>
#include<phy_mem.h>
#include<virt_mem.h>
#include <task.h>
#include <sys/gdt.h>

uint64_t elf_start;


void print_posix_header( struct posix_header_ustar *p){

  printf("Tarfs header\n");
  printf("Name: %s, Mode: %s, Uid: %s\n",  p->name, p->mode, p->uid);
  printf("Gid: %s, Size: %s, Typeflag: %s\n", 
         p->gid, p->size, p->typeflag);

  //  printf("\n Posix header :  %s :%s :%s ",p->name,p->size,p->typeflag);

}

extern uint64_t oct_to_dec(char *oct);
extern char _binary_tarfs_start, _binary_tarfs_end;

struct posix_header_ustar* get_elf_file(char* filename, Exe_Format *exeFormatAddr) {
  char* header =  (char*)(&_binary_tarfs_start);
  char *end =  (char*)(&_binary_tarfs_end);

// while(header < (struct posix_header_ustar*)(&_binary_tarfs_end))
  struct posix_header_ustar * h = (struct posix_header_ustar*)header;
   while(header < end && matchString(h->name, filename) != 0) {
    int sz = oct_to_dec(h->size);
    printf("Name: %s Size: %d Cur: %p\n", h->name, sz, h);
    int jump = (sizeof(struct posix_header_ustar) + sz);
    if (jump % 512 != 0)
      jump += (512 - jump % 512);
    header = header + jump;
    h = (struct posix_header_ustar*)header;
   }
   if (header >= end) {
    printf("ERROR: %s not found in tarfs\n");
    return NULL;
   }
   h++;
  
   programHeader pdr;
   elf_start = (uint64_t) h; 
   int x = Parse_ELF_Executable((char*)h, oct_to_dec((h-1)->size),exeFormatAddr,&pdr);
   printf("Status: %d\n", x);

   return h;
}

void map_exe_format(Exe_Format* exeFormatAddr){

  printf("\nStart Address %x",exeFormatAddr->entryAddr);
  int i =0, cur_vma_idx = VMA_SEGMENT_START;
  for (; i<exeFormatAddr->numSegments; i++){
    printf("I :%d\n", i);
    
    struct Exe_Segment segment  = (struct Exe_Segment)exeFormatAddr->segmentList[i];
    uint64_t start = segment.vaddr;
    uint64_t end = segment.vaddr + segment.sizeInMemory;
   
    
    // printf("\n Start: %x End: %x", start ,end);
   
    uint64_t pageNeeded = 0;
    uint64_t least_start = 0;
    uint64_t max_end = 0;

    if(end-start !=0){

      
      least_start = (start/0x1000)*0x1000;
      max_end = (end/0x1000)*0x1000 + 0x1000;

      //   printf("\n least_start : %x",least_start);
      // printf("\n max_end : %x",max_end);
      pageNeeded = (max_end - least_start)/0x1000;
      // printf("\nPage Needed :%x",pageNeeded);
   
    }
    printf("Seg: %d least_start: %x, max_end: %x\n", i, least_start, max_end);
    if(pageNeeded !=0){
     current_task->vma[cur_vma_idx].start_addr = least_start;
     current_task->vma[cur_vma_idx].end_addr = max_end;
     cur_vma_idx++;
        while(pageNeeded !=0){

	  void *physicalAddress = page_alloc();
	  //  printf("\n Physical Address :%x",physicalAddress);
	 
	  map_process(least_start,(uint64_t)physicalAddress);
	  
	  pageNeeded -= 1;
	  least_start += 0x1000;

       }

	uint64_t ondiskstart = segment.offsetInFile + elf_start;

	uint64_t ondiskfinish = segment.offsetInFile + elf_start + segment.sizeInMemory;
       	printf("\n ondiskstart: %x odiskfinish: %x", ondiskstart,ondiskfinish);
	
	uint64_t size = segment.sizeInMemory;

	char *ondisk =  (char *) ondiskstart;
	char *vadd = (char *) start;
	//	printf("\nvadd %x",vadd);
	while(size){

	  *vadd = *ondisk;
	  vadd++;
	  ondisk++;

	  size--;
	}
	
	//	printf("\nvadd %x",vadd);
     
    }
 }

  //moving the instruction pointer
  
  //  __asm__ __volatile__("movq %0,%%rip;"::"g"(entryAddress));
 // jump_to_start(entryAddress);


}



int matchString( char *s , char *t){

  int ret = 0;

  while (!(ret = *(unsigned char *) s - *(unsigned char *) t) && *t){
    ++s;
    ++t;
  }

  if (ret < 0){
    ret = -1;
  }
  else if (ret > 0){
    ret = 1 ;
  }

  return ret;

}
