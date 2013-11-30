#include <virt_mem.h>
#include <task.h>
#include <stdio.h>
#include <sys/gdt.h>
#include <virt_mem.h>
#include <pic.h>

extern void cpu_write_cr3(uint64_t val);
extern void cpu_write_rsp(uint64_t val);
extern uint64_t i_virt_to_phy(uint64_t virt);
extern page_directory_t* cur_pml4e_virt;
extern struct tss_t tss;
extern volatile task_t *current_task;
extern volatile task_t *ready_queue;


//void store_trap_frame() {
void switch_task()
{
   // If we haven't initialised tasking yet, just return.
   if (!current_task) {
     printf("Havent initilized yet\n");
       return;
   }
  
  //tss.rsp is where the stack begins. We need to move to the position just before timer is called, so that we can pop all the registers
  //current_task->rsp = tss.rsp0 - TSS_RSP_OFFSET;
//  __asm volatile( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory" );
 __asm__ __volatile__("movq %%rsp, %0;":"=g"(current_task->rsp));
   
  //printf("Switching\n");

  // Get the next task to run.
  current_task = get_next_ready_task();
  // If we fell off the end of the linked list start again at the beginning.
  //When it was interrupted the last time, we stored the position from where we can now pop all the stuff

  tss.rsp0 = current_task->tss_rsp;
  cur_pml4e_virt = current_task->pml4e;
  printf("Try to switch to process %d\n", current_task->id); 
  cpu_write_cr3(i_virt_to_phy((uint64_t)cur_pml4e_virt));
//  cpu_write_rsp(current_task->rsp);
  __asm__ __volatile__( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory" );
  current_task->run_time += SCHEDULE_FREQUENCY;
  if (current_task->run_time == SCHEDULE_FREQUENCY && current_task->id != 1)  {
    printf("Returning now");
    __asm__ __volatile__(
            "popq %r11;\n"
            "popq %r10;\n"
            "popq %r9;\n"
            "popq %r8;\n"
            "popq %rdi;\n"
            "popq %rsi;\n"
            "popq %rdx;\n"
            "popq %rcx;\n"
            "popq %rbx;\n"
            "popq %rax;\n");

    port_outb(0xA0, 0x20);
    port_outb(0x20, 0x20);

    __asm__ __volatile__("iretq");

  }
  printf("Switched to task: %d Num tasks: %d\n", current_task->id, numtasks());   //printf("Swithing 2\n");
}

