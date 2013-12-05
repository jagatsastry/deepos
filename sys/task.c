#include <defs.h>
#include <stdio.h>
#include <task.h>
#include <virt_mem.h>
#include <sys/gdt.h>
#include <stdlib.h>

extern void _enter_user_mode();
extern uint64_t i_virt_to_phy(uint64_t phy);
extern struct tss_t tss;

typedef uint32_t taskid_t;

extern uint64_t kern_stack_virt;

void enter_user_mode () {
  printf("Entering user mode\n");
  *((uint64_t*)kern_stack_virt) = 0;
  tss.rsp0 = kern_stack_virt;
  printf("Stack addr: %x\n", tss.rsp0);
//  __asm__ __volatile__ ("cli")
  
  //while(1);
  /*
  __asm__ __volatile__("push %ax;");//::"r"(&tem));
  __asm__ __volatile__("mov $0x2B,%ax;");//::"r"(&tem));
  __asm__ __volatile__("ltr %ax");
  __asm__ __volatile__("pop %ax;");//::"r"(&tem));
*/
  _enter_user_mode();  
  //*((uint64_t*)virtual_address_k) = 0;
  printf("In user mode\n");
}



volatile task_t *current_task;

task_t ready_queue[MAX_TASKS];

// Some externs are needed to access members in paging.c...
extern page_directory_t *kern_pml4e_virt;
extern page_directory_t *cur_pml4e_virt;

extern uint64_t initial_rsp;
extern uint64_t _read_rip();
extern void* i_virt_alloc();

// The next available process ID.
uint32_t next_pid = 1;
extern page_directory_t* clone_page_directory(page_directory_t* tab_src, int level);


task_t* get_next_ready_task() {
  uint32_t startIdx = (current_task?current_task->index + 1:0);
   
  int i = 1;
  for (i = 0; i < MAX_TASKS; i++) {
    task_t *task = &ready_queue[(startIdx + i) %MAX_TASKS];
    //printf("Task %d Status %d\n", task->id, task->STATUS);
    if (task->STATUS == TASK_READY)
      return task;
  }
  printf("\nERR: NO READY TASK\n");
  while(1);
}

task_t* get_next_free_task() {
  int i = 0;
  for (i = 0; i < MAX_TASKS; i++) {
    if (ready_queue[i].STATUS == TASK_FREE)
      return ready_queue + i;
  }
  printf("\nERR: TASK LIMIT EXCEEDED\n");
  while(1);
}

task_t* get_children(pid_t pid) {
  task_t *children = (task_t*)0;
  int i;
  printf("Searching for children\n");
  for (i = 0; i < MAX_TASKS; i++) {
    if (ready_queue[i].parent && ready_queue[i].parent->id == pid) {
      printf("Found a child %d for parent %d", ready_queue[i].id, pid);
      task_t* child =  ready_queue + i;
      child->next = children;
      children = child;
    }
  }
  return children;
}

task_t* get_task(pid_t pid) {
  int i;
  for (i = 0; i < MAX_TASKS; i++) {
    if (ready_queue[i].id == pid)
      return ready_queue + i;
  }
  //printf("\nERR: Task %d not found\n", pid);
  //while(1);
  return 0;
}
extern char *stack;
extern void print_current_task();
void initialize_tasking()
{
  //__asm__ __volatile__("cli");
   int i = 0;
   for (i = 0; i < MAX_TASKS; i++)
     ready_queue[i].index = i;
   current_task = get_next_free_task();
   current_task->id = next_pid++;
   current_task->pml4e = cur_pml4e_virt;
   current_task->STATUS = TASK_READY;
   current_task->program_name = "init";
   for (i = 0; i < 10; i++)
     current_task->vma[i].start_addr = NULL;
   //uint64_t virt_u_rsp = (uint64_t)i_virt_alloc();
   current_task->vma[VMA_USER_STACK_IDX].start_addr = (uint64_t)stack;
   current_task->vma[VMA_KERNEL_STACK_IDX].start_addr = (uint64_t)i_virt_alloc();
   current_task->vma[VMA_HEAP_IDX].start_addr = (uint64_t)i_virt_alloc();

   for(i = 0; i < VMA_SEGMENT_START; i++)
     current_task->vma[i].end_addr = current_task->vma[i].start_addr + 4095;

    printf("Init: User: %x, Kernel: %x\n", current_task->vma[VMA_KERNEL_STACK_IDX].end_addr, current_task->vma[VMA_HEAP_IDX].end_addr);

   current_task->rsp  = current_task->vma[VMA_KERNEL_STACK_IDX].end_addr;
   current_task->current_heap_ptr  = (void*)current_task->vma[VMA_HEAP_IDX].start_addr;
   __asm__ __volatile__ ("movq %%rsp, %0;" : "=g"((uint64_t)current_task->u_rsp));

   printf("Setting ltr");

   int a = 0x28;
   __asm__ __volatile__("movq %0,%%rax;\n"
               "ltr (%%rax);"::"r"(&a));

   printf("Multi task system initialized\n");
   print_current_task();
}

uint32_t getpid() {
  return current_task->id;
}

pid_t numtasks() {
  pid_t num = 0, i = 0;
  for (i = 0; i < MAX_TASKS; i++) {
    if (ready_queue[i].STATUS != TASK_FREE && ready_queue[i].STATUS != TASK_ZOMBIE)
      num++;
  }
  return num;
}

