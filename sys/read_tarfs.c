#include<stdio.h>
#include<defs.h>
#include<sys/tarfs.h>
#include<elf.h>
#include<phy_mem.h>
#include<virt_mem.h>

struct Exe_Format exeFormat;
programHeader pdr;
uint64_t elf_start;


void print_posix_header( struct posix_header_ustar *p){

  printf("Tarfs header\n");
  printf("Name: %s, Mode: %s, Uid: %s\nGid: %s, Size: %s, Typeflag: %s\n", 
        p->name, p->mode, p->uid, p->gid, p->size, p->typeflag);

  //  printf("\n Posix header :  %s :%s :%s ",p->name,p->size,p->typeflag);

}



struct posix_header_ustar* get_elf_file(char *binary_tarfs_start) {


  // printf("\n  %p",binary_tarfs_start);



 struct  posix_header_ustar *tar_p=(struct posix_header_ustar *)(binary_tarfs_start);
 print_posix_header(tar_p);

  char *z = tar_p->size;

   printf("\n size in %d",atoi(z));
 


//char *c = binary_tarfs_start;
//char *p = "bin";
   char *value = "bin/hello";

//printf("\n string match %d: ",matchString( p , temp));
// printf("\n %s",temp);
   char *temp = binary_tarfs_start;
  
   while(matchString(tar_p->name,value)!=0){

     temp = temp + 512  + atoi(tar_p->size);
        tar_p=(struct posix_header_ustar *)temp;
		print_posix_header(tar_p);

 }
 printf("Matched! %s\n", tar_p->name);
     temp = temp+ 512;
   //   printf("\n temp address %p",temp);


   // printf("\n size : %d",atoi(tar_p->size) );
     //   printf("\n Header:");



     elf_start = (uint64_t) temp; 
     int x = Parse_ELF_Executable((char*)temp, atoi(tar_p->size),&exeFormat,&pdr);
     printf("Status: %d\n", x);
     //     printf("\n temp: %x",temp);
         //printf("\n exe format :%d\n", exitStatus);


    // printf("\n Num Segemts: %d , Entry address : %p", exeFormat.numSegments, exeFormat.entryAddr);

    
    // printf("\n Phdr address %x ",&pdr);

    //   print_exe_format(&exeFormat);
  
    //print_p_format(&exeFormat,&pdr);


   return tar_p;

  /*

c =c+512;
struct  posix_header_ustar *tar_p1=(struct posix_header_ustar *)(c);
print_posix_header(tar_p1);
c= c+512;
struct  posix_header_ustar *tar_p2=(struct posix_header_ustar *)(c);
print_posix_header(tar_p2);
c =c+512;
char * p1 =c;
for ( ; p1<c+200;p1++){
  printf("%c",*p1);
 }
  */
   }


void map_exe_format(){

  printf("\nStart Address %x",exeFormat.entryAddr);
  uint64_t entryAddress = (uint64_t)exeFormat.entryAddr;
  int i =0;
  for (; i<exeFormat.numSegments; i++){
    
    struct Exe_Segment segment  = (struct Exe_Segment)exeFormat.segmentList[i];
    // printf("\n  offset in File :%d",segment.offsetInFile);
    // printf("\n Segment length File %d",segment.lengthInFile);
    // printf("\n Segment start Address %x",segment.startAddress);
    // printf("\n Segment size in Memory %x",segment.sizeInMemory);
    // printf("\n Segment prot flags %x",segment.protFlags);
    // printf("\n vAddr %x:",segment.vaddr);

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
  
  __asm__ __volatile__("jmp %0"::"r"(entryAddress));


}

uint64_t get_entry_address(){
  
  return exeFormat.entryAddr;

}


int atoi(char *str)
{
  int res = 0; // Initialize result
 
  // Iterate through all characters of input string and update result
  for (int i = 0; str[i] != '\0'; ++i)
    res = res*10 + str[i] - '0';
 
  // return result.
  return res;
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
