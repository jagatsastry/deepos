#include<stdio.h>
#include<defs.h>
#include<sys/tarfs.h>
#include<elf.h>
#include<phy_mem.h>
#include<virt_mem.h>
#include <task.h>
#include <sys/gdt.h>

struct Exe_Format exeFormat;
programHeader pdr;
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

struct posix_header_ustar* get_elf_file(char* filename) {

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
  
   elf_start = (uint64_t) h; 
   int x = Parse_ELF_Executable((char*)h, oct_to_dec((h-1)->size),&exeFormat,&pdr);
   printf("Status: %d\n", x);

   return h;
}

void jump_to_start(uint64_t entryAddress) {
  __asm__ __volatile__("jmp %0"::"r"(entryAddress));
}

void map_exe_format(){

  printf("\nStart Address %x",exeFormat.entryAddr);
  int i =0;
  for (; i<exeFormat.numSegments; i++){
    printf("I :%d\n", i);
    
    struct Exe_Segment segment  = (struct Exe_Segment)exeFormat.segmentList[i];
    /*
     printf("\n  offset in File :%d",segment.offsetInFile);
     printf("\n Segment length File %d",segment.lengthInFile);
     printf("\n Segment start Address %x",segment.startAddress);
     printf("\n Segment size in Memory %x",segment.sizeInMemory);
     printf("\n Segment prot flags %x",segment.protFlags);
     printf("\n vAddr %x:",segment.vaddr);
*/
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
extern void* i_virt_alloc();

uint64_t temp_rsp;
int kexecvpe_wrapper(char* filename, int argc, char *argv[], char *argp[], int kernel) {
  printf("Running kexecvpe of %s\n", filename);
  //__asm__ __volatile__("cli");
  struct  posix_header_ustar *tar_p = get_elf_file(filename);
  if (tar_p == NULL)
    return -1;
  print_posix_header(tar_p);
  char* x = tar_p->size;
  printf("\n%s\n", x);
  printf("\n%s\n", x);
  printf("hi\n");
  map_exe_format();
  printf("Preparing the stack\n");
  __asm__ __volatile__("movq %%rsp, %0;":"=g"(temp_rsp));

  current_task->u_rsp = (uint64_t)i_virt_alloc() + 4096 - 1;

  current_task->rsp = (uint64_t)i_virt_alloc() + 4096 - 1;
  current_task->tss_rsp = (uint64_t)current_task->rsp;
  tss.rsp0 = (uint64_t)current_task->rsp;
  printf("Move the stack temporarily\n");
  __asm__ __volatile__( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory" );

  /*if (kernel) {
  __asm volatile("pushq $0x20\n\t"
                 "pushq %0\n\t"
                 "pushq $0x200292\n\t"
                 "pushq $0x08\n\t"
                 "pushq %1\n\t"
       : :"c"(current_task->u_rsp),"d"((uint64_t)exeFormat.entryAddr) :"memory");
  } else {*/
  __asm volatile("pushq $0x23\n\t"
                 "pushq %0\n\t"
                 "pushq $0x200292\n\t"
                 "pushq $0x1b\n\t"
                 "pushq %1\n\t"
       : :"c"(current_task->u_rsp),"d"((uint64_t)exeFormat.entryAddr) :"memory");
  //}
  __asm__ __volatile__ ("iretq":::"memory");


  
  __asm__ __volatile__ (
                  "pushq %rax;\n"
                  "pushq %rbx;\n"
                  "pushq %rcx;\n"
                  "pushq %rdx;\n"
                  "pushq %rsi;\n"
                  "pushq %rdi;\n"
                  "pushq %rbp;\n"
                  "pushq %r8;\n"
                  "pushq %r9;\n"
                  "pushq %r10;\n"
                  "pushq %r11;\n"
                  "pushq %r12;\n"
                  "pushq %r13;\n"
                  "pushq %r14;\n"
                  "pushq %r15;\n");
  current_task->run_time = 0; 
 __asm__ __volatile__("movq %%rsp, %0" : "=r"(current_task->rsp));
  printf("Current rsp of process %d: %x line: %d\n", current_task->id, current_task->rsp, __LINE__);
  __asm__ __volatile__( "movq %0, %%rsp ": : "m"(temp_rsp) : "memory" );
  printf("Entry addr: %x\n", (uint64_t)exeFormat.entryAddr);
  switch_task();
  //__asm__ __volatile__ ("iretq":::"memory");
 // __asm__ __volatile__("sti");
  return 0;
}

int kexecvpe(char* filename, int argc, char *argv[], char *argp[]) {
  return kexecvpe_wrapper(filename, argc, argv, argp, 0);
}


