#include<stdio.h>
#include <string.h>
#include<defs.h>
#include <pic.h>
#include<sys/tarfs.h>
#include<elf.h>
#include<phy_mem.h>
#include<virt_mem.h>
#include <task.h>
#include <sys/gdt.h>


extern void* i_virt_alloc();
extern void print_current_task();

extern uint64_t i_virt_to_phy(uint64_t virt);

uint64_t temp_rsp;
int kexecvpe(char* filename, int argc, char *argv[], char *argp[]) {
  struct Exe_Format exeFormat;
  if (DEBUG) printf("Running kexecvpe of %s\n", filename);
  struct  posix_header_ustar *tar_p = get_elf_file(filename, &exeFormat);
  if (tar_p == NULL) {
    if (DEBUG) printf ("Going back from kexecve\n");
    return -1;
  }
  print_posix_header(tar_p);
  char* x = tar_p->size;
  if (DEBUG) printf("\n%s\n", x);
  if (DEBUG) printf("\n%s\n", x);
  if (DEBUG) printf("hi\n");
  int i = 0;
  for (; i < 10; i++)
    current_task->vma[i].start_addr = 0;
  current_task->vma[VMA_USER_STACK_IDX].start_addr = (uint64_t)i_virt_alloc(); //User stack
  current_task->vma[VMA_KERNEL_STACK_IDX].start_addr = (uint64_t)i_virt_alloc(); //Kernel stack
  
  for(i = 0; i < VMA_SEGMENT_START; i++)
    current_task->vma[i].end_addr = current_task->vma[i].start_addr + 4095;

  map_exe_format(&exeFormat);
  if (DEBUG) printf("Preparing the stack\n");
  __asm__ __volatile__("movq %%rsp, %0;":"=g"(temp_rsp));
  current_task->execEntryAddress = exeFormat.entryAddr;
  current_task->u_rsp = (uint64_t)current_task->vma[VMA_USER_STACK_IDX].end_addr;
  current_task->rsp = (uint64_t)current_task->vma[VMA_KERNEL_STACK_IDX].end_addr;
  if (DEBUG) printf("In execve PID: %d. USP %x RSP %x\n", current_task->id, current_task->u_rsp, current_task->rsp);
  current_task->tss_rsp = (uint64_t)current_task->rsp;
  strcpy((char*)current_task->program_name, filename);
  tss.rsp0 = (uint64_t)current_task->rsp;
  if (DEBUG) printf("Move the stack temporarily\n");
  print_current_task();

  uint64_t phy_pml4e = i_virt_to_phy((uint64_t)current_task->pml4e);

  __asm__ __volatile__( "movq %0, %%cr3" : : "r" (phy_pml4e) );
  port_outb(0xA0, 0x20);
  port_outb(0x20, 0x20);
  __asm__ __volatile__( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory" );
  __asm volatile("pushq $0x23\n\t"
                 "pushq %0\n\t"
                 "pushq $0x200292\n\t"
                 "pushq $0x1b\n\t"
                 "pushq %1\n\t"
       : :"c"(current_task->u_rsp),"d"((uint64_t)current_task->execEntryAddress) :"memory");
      current_task->run_sessions_count++;
          current_task->run_time += SCHEDULE_FREQUENCY;

  __asm__ __volatile__ ("iretq":::"memory");
  return 0;
}


