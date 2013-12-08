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
extern void  create_shell();
extern struct page_directory_t *kern_pml4e_virt, *kern_pml4e_phy;
extern struct page_directory_t *cur_pml4e_virt;
extern uint64_t cpu_read_cr3();
extern void shell();
extern uint64_t get_phy_addr(uint64_t addr, page_directory_t* pml4e);
extern int kexecvpe_wrapper(char* filename, int argc, char *argv[], char *argp[], int kernel);
extern int exec(char*);
extern uint32_t fork_wrapper(int kernel);
extern void clear_line23();

void init_kernel(uint32_t* modulep, void* kernmem, void* physbase, void* physfree) {
  printf("Booting Deep-OS\n");
  printf("Initializing interrupt handler\n");
  if(ENABLE_INTR) {
    init_pics();
    idtStart();
  }
  printf("Initializing memory manager\n");
  init_mem_mgmt(modulep, kernmem, physbase, physfree);

  if (DEBUG) printf("Kernmem virt: %x\n", kernmem);
  if (DEBUG) printf("Kernmem before: %x\n", get_phy_addr((uint64_t)kernmem, 
      kern_pml4e_virt));

  printf("Initializing process manager\n");
  initialize_tasking();
  printf("Creating deep shell\n");
  create_shell();

  if (DEBUG) printf("Back after a simple switch. Current PID %d\n", getpid());
  while(1);
}
