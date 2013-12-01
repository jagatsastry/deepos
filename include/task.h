#ifndef TASK_H
#define TASK_H
#include <virt_mem.h>
#include <stdlib.h>


#define SCHEDULE_FREQUENCY 1000

#define TASK_FREE  0
#define TASK_READY 1
#define TASK_WAITING_ON_PID 2
#define TASK_SLEEPING 3
#define TASK_ZOMBIE 3

#define MAX_TASKS 100

// This structure defines a 'task' - a process.
typedef struct task
{
   struct task* parent;
   uint32_t id;                // Process ID.
   uint16_t index;                // Process ID.
   uint32_t exit_status;
   uint32_t waiting_pid_exit_status;
   uint8_t STATUS;
   uint32_t sleeping_time;
   uint32_t pid_waiting_for;
   uint32_t run_time;

   uint32_t run_sessions_count; //Number of times it entered switch_task
   uint64_t rsp;       // Kernel stack and base pointers.
   uint64_t tss_rsp;
   uint64_t u_rsp; //User stack and base pointer

   uint64_t heap_ptr; //pointer to heap

   uint64_t code_start;
   uint64_t code_end; //Where the process image starts and ends
   
   page_directory_t *pml4e; // Page directory.
} task_t;

extern volatile task_t *current_task;
extern task_t ready_queue[MAX_TASKS];

// Initialises the tasking system.
void initialize_tasking();

task_t* get_next_ready_task();
task_t* get_next_free_task();
task_t* get_task(int pid) ;
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

#endif
