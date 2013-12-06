#include <virt_mem.h>
#include <task.h>
#include <stdio.h>
#include <sys/gdt.h>
#include <virt_mem.h>
#include <pic.h>
#include <timer.h>

extern void cpu_write_cr3(uint64_t val);
extern void cpu_write_rsp(uint64_t val);
extern uint64_t i_virt_to_phy(uint64_t virt);
extern page_directory_t* cur_pml4e_virt;
extern struct tss_t tss;

#define DEBUG 1
void update_waiting_and_sleeping_tasks() {
  int i = 0;
  for (i = 0; i < MAX_TASKS; i++) {
    task_t *task = &ready_queue[i];
    if (task->STATUS == TASK_SLEEPING) {
      task->sleeping_time -= TICK_PER_SECOND;
      if (task->sleeping_time  <= 0) {
        task->sleeping_time = 0;
        task->STATUS = TASK_READY;
      }
    }
    if (task->STATUS == TASK_WAITING_ON_PID) {
      task_t *wait_task = (task_t*)0;
      if (task->pid_waiting_for == -1) {
        wait_task = get_children(task->id);
        while(wait_task && wait_task->STATUS != TASK_ZOMBIE) 
          wait_task = wait_task->next;
      }
      else 
        wait_task = get_task(task->pid_waiting_for);
      printf("Out of zombie search\n");
      //if (!wait_task)
      //  task->STATUS == TASK_READY;
      //Once a zombie, always a zombie. Kill them later
      if ( wait_task && wait_task->STATUS == TASK_ZOMBIE) {
        printf("Found a zombie kid %d on whom %d is waiting\n", wait_task->id, task->id);
        task->pid_waiting_for = wait_task->id;
        task->STATUS = TASK_READY;
        task->waiting_pid_exit_status = wait_task->exit_status;

/*        int idx = wait_task->index;
        *wait_task = 0;
        wait_task->index = idx;*/
     }
    }
  }
}

extern void print_current_task();
//void store_trap_frame() {
void switch_task()
{
   if (!current_task) {
     if(DEBUG) printf("Havent initilized yet\n");
       return;
   }
  
  if (current_task->run_time >= SCHEDULE_FREQUENCY)   {
   __asm__ __volatile__("movq %%rsp, %0;":"=g"(current_task->rsp));
   if(DEBUG) printf("Updated rsp of %d to %x: %d\n", current_task->id, current_task->rsp, __LINE__);
  }
  int oldPid = current_task->id;  
  if(DEBUG) printf("Switching\n");

  current_task = get_next_ready_task();

  print_current_task();
  cur_pml4e_virt = current_task->pml4e;
  uint64_t phy_pml4e = i_virt_to_phy((uint64_t)cur_pml4e_virt);
  if (DEBUG) printf("Switching from %d to %d\n", oldPid, current_task->id);
  tss.rsp0 = current_task->tss_rsp;
  if(DEBUG) printf("Try to switch to process %d\n", current_task->id);
  if(DEBUG) printf("%x\n", phy_pml4e);
    printf("Task %d: USP %x, RSP %x\n", current_task->id, current_task->u_rsp, current_task->rsp);
  __asm__ __volatile__( "movq %0, %%cr3" : /* no output */ : "r" (phy_pml4e) );
  __asm__ __volatile__( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory" );
  if (current_task->rip) {
      current_task->run_sessions_count++;
      current_task->run_time += SCHEDULE_FREQUENCY;
 
      __asm__ __volatile__( "movq %0, %%rcx; \
                        movq $0x12345, %%rax; \
                        jmpq *%%rcx" : : "r"(current_task->rip));
  }
  if (current_task->run_time < SCHEDULE_FREQUENCY) {// && current_task->id != 1)  {
    if(DEBUG) printf("Returning now");
    __asm__ __volatile__(
            "popq %r15;\n"
            "popq %r14;\n"
            "popq %r13;\n"
            "popq %r12;\n"
            "popq %r11;\n"
            "popq %r10;\n"
            "popq %r9;\n"
            "popq %r8;\n"
            "popq %rbp;\n"
            "popq %rdi;\n"
            "popq %rsi;\n"
            "popq %rdx;\n"
            "popq %rcx;\n"
            "popq %rbx;\n"
            "popq %rax;\n");

    port_outb(0xA0, 0x20);
    port_outb(0x20, 0x20);

    if(DEBUG) printf("Proc: %d . Run session ct: %d. Run time: %d, Schedule freq: %d\n", 
        current_task->id, current_task->run_sessions_count,
        current_task->run_time, SCHEDULE_FREQUENCY);
    current_task->run_sessions_count++;
    current_task->run_time += SCHEDULE_FREQUENCY;
    __asm__ __volatile__("iretq" ::: "memory");

  }
  if(DEBUG) printf("Switched to task: %d Num tasks: %d\n", current_task->id, numtasks());   //printf("Swithing 2\n");
  current_task->run_time += SCHEDULE_FREQUENCY;
  current_task->run_sessions_count++;
}

