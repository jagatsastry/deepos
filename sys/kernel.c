#include<stdio.h>
#include<idt.h>
#include<utilities.h>
#include<pic.h>
#include <phy_mem.h>
#include <virt_mem.h>
#include <sys/tarfs.h>

#define ENABLE_INTR 0
void init_mem_mgmt(uint32_t* modulep, void* kernmem, void* physbase, void* physfree);
void run_elf() ;

  

void init_kernel(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {
  if(ENABLE_INTR) {
    init_pics();
    idtStart();
  }
  init_mem_mgmt(modulep, kernmem, physbase, physfree);
  printf("Booting Deep-OS\n");
  while(1);
//  run_elf();
}

void run_elf() {
  struct  posix_header_ustar *tar_p= get_elf_file(&_binary_tarfs_start);
  print_posix_header(tar_p);
  char* x = tar_p->size;
  printf("\n%s\n", x);
  printf("\n%s\n", x);
  printf("hi\n");
  map_exe_format();
}
