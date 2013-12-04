#include<stdio.h>
#include<idt.h>
#include<utilities.h>
#include<pic.h>
#include <phy_mem.h>
#include <virt_mem.h>
#include <sys/tarfs.h>
#include <task.h>

#define ENABLE_INTR 1
void init_mem_mgmt(uint32_t* modulep, void* kernmem, void* physbase, void* physfree);

extern page_directory_t* clone_page_directory(page_directory_t* tab_src, int level) ;

extern uint64_t i_virt_to_phy(uint64_t virt);
  
extern struct page_directory_t *kern_pml4e_virt, *kern_pml4e_phy;
extern struct page_directory_t *cur_pml4e_virt;
extern uint64_t cpu_read_cr3();
extern void shell();

extern uint64_t get_phy_addr(uint64_t addr, page_directory_t* pml4e);
extern int kexecvpe_wrapper(char* filename, int argc, char *argv[], char *argp[], int kernel);
  
extern int exec(char*);
extern uint32_t fork_wrapper(int kernel);

void init_kernel(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {
  if(ENABLE_INTR) {
    init_pics();
    idtStart();
  }
  init_mem_mgmt(modulep, kernmem, physbase, physfree);

  printf("Booting Deep-OS\n");
  //setup_kernel_stack();
  printf("Kernmem virt: %x\n", kernmem);
  printf("Kernmem before: %x\n", get_phy_addr((uint64_t)kernmem, 
      kern_pml4e_virt));

  initialize_tasking();
  create_shell();

  printf("Back after a simple switch. Current PID %d\n", getpid());
  while(1);
  //while(1) { 
  //printf("Back after a simple switch. Current PID %d\n", getpid());
  //};
}
/*
void run_elf() {
  struct  posix_header_ustar *tar_p= get_elf_file(&_binary_tarfs_start);
  print_posix_header(tar_p);
  char* x = tar_p->size;
  printf("\n%s\n", x);
  printf("\n%s\n", x);
  printf("hi\n");
  map_exe_format();
}
*/
