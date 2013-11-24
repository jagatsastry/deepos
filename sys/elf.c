#include <elf.h>
#include <defs.h>
#include <stdio.h>
/*From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 * and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int elfDebug=1;

int strncmp(const char *s1, const char *s2, uint32_t n)
{
  for ( ; n > 0; s1++, s2++, --n)
    if (*s1 != *s2)
      return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
    else if (*s1 == '\0')
      return 0;
  return 0;
}

int Parse_ELF_Executable(char *exeFileData, uint64_t exeFileLength,
			 struct Exe_Format *exeFormat , programHeader * phdr)
{
  //printf("Len: %d\n", exeFileLength);
  //printf("\nhi\n");
  elfHeader *hdr;
  // programHeader *phdr;
  int i;

  hdr = (elfHeader *) exeFileData;

  //  printf("\n Inside ELF: [  %x: %x: %x: \n %x: %d: %d:",hdr->type,hdr->version,hdr->entry,hdr->phoff,hdr->phnum,hdr->shnum);
  /*
     
	   unsigned char ident[16];
	   unsigned short type;
	   unsigned short machine;
	   unsigned int version;
	   unsigned int entry;
	   unsigned int phoff;
	   unsigned int sphoff;
	   unsigned int flags;
	   unsigned short ehsize;
	   unsigned short phentsize;
	   unsigned short phnum;
	   unsigned short shentsize;
	   unsigned short shnum;
	   unsigned short shstrndx;
	 
  */



  if (exeFileLength < sizeof(elfHeader) ||
      strncmp(exeFileData, "\x7F" "ELF", 4) != 0) {
    if (elfDebug)
      printf("Not an ELF executable\n");
    return 1;
  }

  if (hdr->phnum > EXE_MAX_SEGMENTS) {
    if (elfDebug)
      printf("Too many segments (%d) in ELF executable\n", hdr->phnum);
    return 1;
  }

  if (exeFileLength < hdr->phoff + (hdr->phnum * sizeof(programHeader))) {
    if (elfDebug)
      printf("Not enough room for program header\n");
    return 1;
  }


  //  printf("\nGetting from Elf:%d",hdr->phnum);
  exeFormat->numSegments = hdr->phnum;
  exeFormat->entryAddr = hdr->entry;

  phdr = (programHeader *) (exeFileData + hdr->phoff);

  printf("\n Phdr address %x, phdr offset: %x, numseg: %d ",phdr, hdr->phoff, hdr->phnum);



  //  printf("\n Phdr address %x ",pdr);
    printf("\n Num Segment of Exe Format : %d", exeFormat->numSegments);
    /*
      int p1 =0;
 for(;p1< hdr->phnum; p1++){

       printf("\n Segments: %x : %x: %x: %x \n :%x :%x :%x : %x ",phdr[p1].type,phdr[p1].offset,phdr[p1].vaddr,phdr[p1].paddr,phdr[p1].fileSize,phdr[p1].memSize,phdr[p1].flags,phdr[p1].alignment);
  }
  */
  
  for (i = 0; i < hdr->phnum; ++i) {
    struct Exe_Segment *segment = &exeFormat->segmentList[i];

    segment->offsetInFile = phdr[i].offset;
    segment->lengthInFile = phdr[i].fileSize;
    segment->sizeInMemory = phdr[i].memSize;
    segment->vaddr = phdr[i].paddr;
    printf("Segment: %d, Offset: %x, Length: %x\n",
          i, segment->sizeInMemory, segment->lengthInFile);
    printf("sizeInMem: %x, vaddr: %x\n",
          segment->sizeInMemory, segment->vaddr);
    if (segment->lengthInFile > segment->sizeInMemory) {
      if (elfDebug)
	                printf
			  ("Segment %d: length in file (%x) exceeds size in memory (%x)\n",
			   i, segment->lengthInFile, segment->sizeInMemory);
      return 1;
    }
  }
  return 0;
}


void print_exe_format(struct Exe_Format* exe_format){

  int i =0;
  printf("\n Num Segment of Exe Format : %d", exe_format->numSegments);
  for(;i<exe_format->numSegments; i++){

    printf("\n Segments: %d : %d: %d: %d",exe_format->segmentList[i].offsetInFile,exe_format->segmentList[i].lengthInFile,exe_format->segmentList[i].startAddress,exe_format->segmentList[i].sizeInMemory );
  }



}

void print_p_format(struct Exe_Format* exe_format,programHeader* pdr){

  printf("\n Phdr address %x ",pdr);
  int i =0;
  printf("\n Num Segment of Exe Format : %d", exe_format->numSegments);
  for(;i<exe_format->numSegments; i++){

    //    printf("\n Segments: %x : %x: %x: %x :%x :%x :%x : %x ",pdr[i].type,pdr[i].offset,pdr[i].vaddr,pdr[i].paddr,pdr[i].fileSize,pdr[i].memSize,pdr[i].flags,pdr[i].alignment);
  }

  /*
  typedef struct {
    unsigned int type;
    unsigned int offset;
    unsigned long vaddr;
    unsigned long paddr;
    unsigned long fileSize;
    unsigned long memSize;
    unsigned long flags;
    unsigned long alignment;
 
    */



}
