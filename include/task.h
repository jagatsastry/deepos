#ifndef TASK_H
#define TASK_H
#include <timer.h>
#include <virt_mem.h>
#include <stdlib.h>


#define VMA_USER_STACK_IDX 0
#define VMA_KERNEL_STACK_IDX 1
#define VMA_HEAP_IDX 2
#define VMA_SEGMENT_START 3


#define SCHEDULE_FREQUENCY 1000 * TICK_PER_SECOND/18

#define TASK_FREE  0
#define TASK_READY 1
#define TASK_WAITING_ON_PID 2
#define TASK_SLEEPING 3
#define TASK_ZOMBIE 4

//4MB limit per process
#define DEFAULT_MEM_LIMIT 4096 * 4096 

#define MAX_TASKS 100

typedef struct vma_t {
  uint64_t start_addr;
  uint64_t end_addr;
  struct vma_t *next;
} vma_t;


#define VMA_DEF_COUNT 100
struct page_directory_t;

// This structure defines a 'task' - a process.
typedef struct task
{
   struct task* parent;
   pid_t id;                // Process ID.
   int new_proc;
   size_t mem_limit;
   size_t current_mem_usage;
   int waiting_for_input;
   uint16_t index;                // Process ID.
   char program_name[64];
   char temp_buffer[64];
   int argc;
   char *argv[32];
   char *envp[32];
   char *path[32];
   char pwd[128];
   uint32_t exit_status;
   uint32_t waiting_pid_exit_status;
   uint8_t STATUS;
   sint64_t sleeping_time;
   uint32_t pid_waiting_for;
   uint32_t time_at_wait;
   uint32_t end_time;
   uint32_t run_time;
   uint32_t just_execd;
   vma_t vma[VMA_DEF_COUNT];
   char *heap_end;
   char *cur_ptr;
   uint32_t run_sessions_count; //Number of times it entered switch_task
   uint64_t rsp;       // Kernel stack and base pointers.
   uint64_t tss_rsp;
   uint64_t u_rsp; //User stack and base pointer

   struct page_directory_t *pml4e; // Page directory.
   struct task *next;  //To be used in a linked list
   uint64_t execEntryAddress; //Entry address of executable when doing exec
   uint64_t temp[10];
   uint64_t rip;
   unsigned char open_files[100];
   unsigned char open_directories[100];

} task_t;

extern volatile task_t *current_task;
extern task_t ready_queue[MAX_TASKS];

// Initialises the tasking system.
void initialize_tasking();

task_t* get_next_ready_task();
task_t* get_next_free_task();
task_t* get_task(pid_t pid) ;
task_t* get_children(pid_t pid);
// Called by the timer hook, this changes the running process.
void switch_task();


// Forks the current process, spawning a new one with a different
// memory space.

// Causes the current process' stack to be forcibly moved to a new location.
void move_stack(void *new_stack_start, uint32_t size);

// Returns the pid of the current process.

void kexit(int status);
void ksleep(uint32_t time);
uint32_t kwaitpid(uint32_t pid, uint32_t *status);
uint32_t kwait(uint32_t *status);
int kexecvpe(char *filename, int argc, char *argv[], char *envp[]);
int kexecvpe_wrapper(char *filename, int argc, char *argv[], char *envp[], int kernelOrNot);
uint32_t kfork();
uint32_t kfork_wrapper(int kernelOrNot);

#endif
