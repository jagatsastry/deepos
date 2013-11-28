#include <defs.h>
#include <stdio.h>
#include <task.h>
#include <virt_mem.h>
#include <sys/gdt.h>

extern void _enter_user_mode();
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

   // Reenable interrupts.
   printf("Multi task system initialized\n");
}

uint32_t getpid() {
  return current_task->id;
}

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

   //page_directory_t *directory = 
   new_task->pml4e = clone_page_directory(cur_pml4e_virt, 4);
  // This will be the entry point for the new process.
  uint64_t rip = _read_rip();
   // We could be the parent or the child here - check.
   if (current_task == parent_task)
   {
     printf("In the parent task\n");
       // We are the parent, so set up the esp/ebp/eip for our child.
       uint64_t rsp; __asm__ __volatile__("movq %%rsp, %0" : "=r"(rsp));
       uint64_t rbp; __asm__ __volatile__("movq %%rbp, %0" : "=r"(rbp));
       new_task->rsp = rsp;
       new_task->rbp = rbp;
       new_task->rip = rip;
       printf("New tas: rsp %x, rbp %x, rip %x\n", rsp, rbp, rip);
       printf("New tas: rsp %x, rbp %x, \n", kern_pml4e_virt, cur_pml4e_virt );
       // All finished: Reenable interrupts.
       __asm__ __volatile__("sti");
       return new_task->id;
   }
   else
   {
     printf("In the child task\n");
       __asm__ __volatile__("sti");
       // We are the child - by convention return 0.
       return 0;
   }
}

extern uint64_t i_virt_to_phy(uint64_t phy);

void switch_task()
{
   // If we haven't initialised tasking yet, just return.
   if (!current_task) {
     printf("Havent initilized yet\n");
       return;
   }

  printf("Switching\n");
  // Read esp, ebp now for saving later on.
  uint64_t rsp, rbp, rip;
  __asm__ __volatile__("movq %%rsp, %0" : "=r"(rsp));
  __asm__ __volatile__("movq %%rbp, %0" : "=r"(rbp));
  printf("rsp is %x\n", rsp);

   // Read the instruction pointer. We do some cunning logic here:
   // One of two things could have happened when this function exits -
   // (a) We called the function and it returned the EIP as requested.
   // (b) We have just switched tasks, and because the saved EIP is essentially
   // the instruction after read_eip(), it will seem as if read_eip has just
   // returned.
   // In the second case we need to return immediately. To detect it we put a dummy
   // value in EAX further down at the end of this function. As C returns values in EAX,
   // it will look like the return value is this dummy value! (0x12345).
   rip = _read_rip();

   // Have we just switched tasks?
  printf("Rip is %x\n", rip);
   if (rip == 0x12345) {
      __asm__ __volatile__("movq %%rsp, %0" : "=r"(rsp));
       printf("Rsp now %x\n", rsp);
       return;
   }

  // No, we didn't switch tasks. Let's save some register values and switch.
   current_task->rip = rip;
   current_task->rsp = rsp;
   current_task->rbp = rbp;
    // Get the next task to run.
   current_task = current_task->next;
   // If we fell off the end of the linked list start again at the beginning.
   if (!current_task) current_task = ready_queue;
   rsp = current_task->rsp;
   rbp = current_task->rbp;
   rip = current_task->rip;
   cur_pml4e_virt = current_task->pml4e;
   // Here we:
   // * Stop interrupts so we don't get interrupted.
   // * Temporarily put the new EIP location in ECX.
   // * Load the stack and base pointers from the new task struct.
   // * Change page directory to the physical address (physicalAddr) of the new directory.
   // * Put a dummy value (0x12345) in EAX so that above we can recognise that we've just
   // switched task.
   // * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
   // the next instruction.
   // * Jump to the location in ECX (remember we put the new EIP in there).
   printf("Setting rsp %x, rip\n", rsp);
   printf("Swithing 1\n");
   __asm__ __volatile__(
     "         \
     cli;                 \
     movq %0, %%rcx;       \
     movq %1, %%rsp;       \
     movq %2, %%rbp;       \
     movq %3, %%cr3;       \
     movq $0x12345, %%rax; \
     sti;                 \
     jmpq *%%rcx           "
     : : "r"(rip), "r"(rsp), "r"(rbp), "r"(i_virt_to_phy((uint64_t)cur_pml4e_virt)));
   printf("Swithing 2\n");
}

