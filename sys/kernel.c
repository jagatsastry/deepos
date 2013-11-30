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

extern uint64_t get_phy_addr(uint64_t addr, page_directory_t* pml4e);
  
extern int exec(char*);
void init_kernel(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {
  if(ENABLE_INTR) {
    init_pics();
    idtStart();
  }
/*
  uint64_t rflags = 0;
  __asm__ __volatile__ (
   "pushfq;\n"
   "popq %%rax\n"
   "movq %%rax, %0" : "=r"(rflags));

  printf("Flags: %x\n", rflags);
  while(1);
  */
  init_mem_mgmt(modulep, kernmem, physbase, physfree);

  printf("Booting Deep-OS\n");
  //setup_kernel_stack();
  printf("Kernmem virt: %x\n", kernmem);
  printf("Kernmem before: %x\n", get_phy_addr((uint64_t)kernmem, 
      kern_pml4e_virt));

/*
  //set_PMLF4_Vaddress((uint64_t)clone_page_directory((uint64_t*)get_PMLF4_Vaddress(), 4));
  cur_pml4e_virt = clone_page_directory(kern_pml4e_virt, 4);

  printf("Kernmem after: %x\n", get_phy_addr((uint64_t)kernmem, cur_pml4e_virt));

  printf("Before: %x\n", cpu_read_cr3());
//  printf("pml4 now: %x\n", pml4);

  __asm__ __volatile__("movq %0, %%cr3":: "b"(i_virt_to_phy((uint64_t)cur_pml4e_virt)));
  printf("\nBack\n");
  printf("After: %x\n", cpu_read_cr3());
  */

  //exec("bin/hello");
  /*
  uint64_t temp_rsp;
   __asm__ __volatile__("movq %%rsp, %0" : "=r"(temp_rsp));
   printf("temprsp: %x\n", temp_rsp);
   uint64_t xxxx;
   __asm__ __volatile__("movq %%rsp, %0" : "=r"(xxxx));
   printf("temprsp: %x\n", xxxx);
   printf("%x %x\n", &temp_rsp, &xxxx);
   */

  initialize_tasking();
  int pid = fork();
  printf("Forked\n");
  if (pid == 0) {
    printf("Execing");
    exec ("bin/hello");
    printf("Back after exec");
    while(1);
  }
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
