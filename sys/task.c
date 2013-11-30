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

volatile task_t *ready_queue;

// Some externs are needed to access members in paging.c...
extern page_directory_t *kern_pml4e_virt;
extern page_directory_t *cur_pml4e_virt;

extern uint64_t initial_rsp;
extern uint64_t _read_rip();
extern void* i_virt_alloc();

// The next available process ID.
uint32_t next_pid = 1;
extern page_directory_t* clone_page_directory(page_directory_t* tab_src, int level);

void initialize_tasking()
{
  /*
  putc('h');
   __asm__ __volatile__("cli");
   __asm__ __volatile__("sti");
  putc('h');*/

   // Relocate the stack so we know where it is.
   // Initialise the first task (kernel task)
   current_task = ready_queue = (task_t*)i_virt_alloc();
   current_task->id = next_pid++;
   current_task->rsp = current_task->rbp = 0;
   current_task->rip = 0;
   current_task->pml4e = cur_pml4e_virt;
   current_task->next = 0;
   //current_task->tss_rsp = (task_t*)i_virt_alloc();

   // Reenable interrupts.
   printf("Multi task system initialized\n");
}

uint32_t getpid() {
  return current_task->id;
}
uint64_t temp_rsp;
uint32_t fork()
{
   // We are modifying kernel structures, and so cannot be interrupted.
   __asm__ __volatile__("cli");

   // Take a pointer to this process' task struct for later reference.
   task_t *parent_task = (task_t*)current_task;

   // Create a new process.
   task_t *new_task = (task_t*)i_virt_alloc(); //kmalloc(sizeof(task_t));

   // Clone the address space.
   new_task->id = next_pid++;
   new_task->rsp = new_task->rbp = 0;
   new_task->rip = 0;
   new_task->next = 0;

   // Add it to the end of the ready queue.
   // Find the end of the ready queue...
   task_t *tmp_task = (task_t*)ready_queue;
   while (tmp_task->next)
       tmp_task = tmp_task->next;
   // ...And extend it.
   tmp_task->next = new_task;
        printf("Hi numtasks: %d\n", numtasks());
        while(1);

   //page_directory_t *directory = 
   new_task->pml4e = clone_page_directory(cur_pml4e_virt, 4);
  // This will be the entry point for the new process.
  uint64_t rip = _read_rip();
   // We could be the parent or the child here - check.
   if (current_task == parent_task)
   {
     printf("In the parent task\n");
       // We are the parent, so set up the esp/ebp/eip for our child.
       __asm__ __volatile__("movq %%rsp, %0" : "=r"(temp_rsp));
       new_task->u_rsp = (uint64_t)i_virt_alloc();
       map_process_specific((uint64_t)new_task->u_rsp, i_virt_to_phy((uint64_t)new_task->u_rsp), new_task->pml4e);
       memcpy((void*)new_task->u_rsp, (void*)temp_rsp, 4096);
       new_task->u_rsp = (uint64_t)i_virt_alloc() + 4096 - 1;

       new_task->rsp = (uint64_t)i_virt_alloc() ;
       map_process_specific((uint64_t)new_task->rsp, i_virt_to_phy((uint64_t)new_task->rsp), new_task->pml4e);
       printf("Move the stack temporarily\n");
       new_task->rsp = new_task->rsp + 4096 - 1;
       current_task->tss_rsp = current_task->rsp;
        __asm__ __volatile__( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory" );
        __asm__ __volatile__ (
                  "pushq %rax;\n"
                  "pushq %rbx;\n"
                  "pushq %rcx;\n"
                  "pushq %rdx;\n"
                  "pushq %rsi;\n"
                  "pushq %rdi;\n"
                  "pushq %r8;\n"
                  "pushq %r9;\n"
                  "pushq %r10;\n"
                  "pushq %r11;\n");
        __asm__ __volatile__("pushq $0x23\n\t"
                       "pushq %0\n\t"
                       "pushq $0x200292\n\t"
                       "pushq $0x1b\n\t"
                       "pushq %1\n\t"
             : :"c"(current_task->u_rsp),"d"((uint64_t)rip) :"memory");

        __asm__ __volatile__( "movq %0, %%rsp ": : "m"(temp_rsp) : "memory" );

       __asm__ __volatile__("sti");
       return new_task->id;
   }
   else
   {
     printf("In the child task\n");
       printf("Returning");
       //__asm__ __volatile__("cli");
       __asm__ __volatile__("sti");
       // We are the child - by convention return 0.
       printf("Returning");
       return 0;
   }
}

int numtasks() {
  volatile task_t* temp = ready_queue;
  int num = 0;
  while(temp) {
    num++;
    temp = temp->next;
  }
  return num;
}


