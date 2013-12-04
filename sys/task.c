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
      printf("Found a parent %d", i);
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

void initialize_tasking()
{
  //__asm__ __volatile__("cli");
   int i = 0;
   for (i = 0; i < MAX_TASKS; i++)
     ready_queue[i].index = i;
   current_task = get_next_free_task();
   current_task->id = next_pid++;
   current_task->rsp  = (uint64_t)i_virt_alloc();
   current_task->pml4e = cur_pml4e_virt;
   current_task->STATUS = TASK_READY;

  printf("Setting ltr");

      int a = 0x28;
               __asm__ __volatile__("movq %0,%%rax;\n"
                           "ltr (%%rax);"::"r"(&a));

    //int a = 0x2B;
   //__asm__ __volatile__("movq %0,%%rax;\n");
   //__asm__ __volatile__("ltr (%%rax);"::"r"(&a));

   //__asm__ __volatile__("movq $0x2B,%rax;");//::"r"(&tem));
   //__asm__ __volatile__("ltr %rax;");
   printf("Multi task system initialized\n");
//  __asm__ __volatile__("sti");

}

uint32_t getpid() {
  return current_task->id;
}

volatile uint64_t temp_rsp;
uint32_t fork_wrapper(int kernel)
{
   // We are modifying kernel structures, and so cannot be interrupted.
   //__asm__ __volatile__("cli");

   // Take a pointer to this process' task struct for later reference.
   task_t *parent_task = (task_t*)current_task;

   // Create a new process.
   task_t *new_task = get_next_free_task();

   // Clone the address space.
   new_task->id = next_pid++;
   new_task->rsp =  0;
   new_task->STATUS = TASK_READY;

   // Add it to the end of the ready queue.
   // Find the end of the ready queue...
    printf("Hi numtasks: %d\n", numtasks());

   //page_directory_t *directory = 
   new_task->pml4e = clone_page_directory(cur_pml4e_virt, 4);
  // This will be the entry point for the new process.
  uint64_t rip = _read_rip();
   // We could be the parent or the child here - check.
   if (current_task == parent_task)
   {
     printf("In the parent task %d\n", current_task->id);
       __asm__ __volatile__("movq %%rsp, %0" : "=r"(temp_rsp));
      printf("RSP is %x\n", temp_rsp);
       // We are the parent, so set up the esp/ebp/eip for our child.
       uint64_t u_rsp  = (uint64_t)i_virt_alloc();
       map_process_specific((uint64_t)u_rsp, i_virt_to_phy((uint64_t)u_rsp), new_task->pml4e);
       memcpy((void*)u_rsp + 2048 - 1, (void*)temp_rsp, 2048);
       new_task->u_rsp = u_rsp + 2048 - 1;

       new_task->rsp = (uint64_t)i_virt_alloc() ;
       map_process_specific((uint64_t)new_task->rsp, i_virt_to_phy((uint64_t)new_task->rsp), new_task->pml4e);
       new_task->rsp = new_task->rsp + 4096 - 1;
       new_task->tss_rsp = new_task->rsp;
       new_task->parent = (task_t*)current_task;

        __asm__ __volatile__( "movq %0, %%rsp ": : "m"(new_task->rsp) : "memory" );
/*
        __asm__ __volatile__("pushq $0x23\n\t"
                       "pushq %0\n\t"
                       "pushq $0x200292\n\t"
                       "pushq $0x1b\n\t"
                       "pushq %1\n\t"
             : :"c"(new_task->u_rsp),"d"((uint64_t)rip) :"memory");

*/
  /*if (kernel) {
  __asm volatile("pushq $0x20\n\t"
                 "pushq %0\n\t"
                 "pushq $0x200292\n\t"
                 "pushq $0x08\n\t"
                 "pushq %1\n\t"
       : :"c"(new_task->u_rsp),"d"((uint64_t)rip) :"memory");
  } else {*/
  __asm volatile("pushq $0x23\n\t"
                 "pushq %0\n\t"
                 "pushq $0x200292\n\t"
                 "pushq $0x1b\n\t"
                 "pushq %1\n\t"
       : :"c"(new_task->u_rsp),"d"((uint64_t)rip) :"memory");
//  }


        __asm__ __volatile__ (
                  "pushq %rax;\n"
                  "pushq %rbx;\n"
                  "pushq %rcx;\n"
                  "pushq %rdx;\n"
                  "pushq %rsi;\n"
                  "pushq %rdi;\n"
                  "pushq %rbp;\n"
                  "pushq %r8;\n"
                  "pushq %r9;\n"
                  "pushq %r10;\n"
                  "pushq %r11;\n"
                  "pushq %r12;\n"
                  "pushq %r13;\n"
                  "pushq %r14;\n"
                  "pushq %r15;\n");

       __asm__ __volatile__("movq %%rsp, %0" : "=r"(new_task->rsp));

printf("Updated rsp for %d to %x: %d\n", new_task->id, new_task->rsp, __LINE__);
        __asm__ __volatile__( "movq %0, %%rsp ": : "m"(temp_rsp) : "memory" );

       //__asm__ __volatile__("sti");
       printf("Set stack for child\n");
       return new_task->id;
   }
   else
   {
     printf("In the child task\n");
       printf("Returning");
   //    __asm__ __volatile__("movq %%rsp, %0" : "=r"(temp_rsp));
    //  printf("RSP is %x\n", temp_rsp);
       //__asm__ __volatile__("cli");
       //__asm__ __volatile__("sti");
       // We are the child - by convention return 0.
       printf("Returning");
       return 0;
   }
}

pid_t numtasks() {
  pid_t num = 0, i = 0;
  for (i = 0; i < MAX_TASKS; i++) {
    if (ready_queue[i].STATUS != TASK_FREE && ready_queue[i].STATUS != TASK_ZOMBIE)
      num++;
  }
  return num;
}

uint32_t fork() {
  return fork_wrapper(0);
}
