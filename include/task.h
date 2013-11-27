#ifndef TASK_H
#define TASK_H
#include <virt_mem.h>


// This structure defines a 'task' - a process.
typedef struct task
{
   uint32_t id;                // Process ID.
   uint64_t rsp, rbp;       // Stack and base pointers.
   uint64_t rip;            // Instruction pointer.
   page_directory_t *pml4e; // Page directory.
   struct task *next;     // The next task in a linked list.
} task_t;

// Initialises the tasking system.
void initialize_tasking();

// Called by the timer hook, this changes the running process.
void switch_task();


// Forks the current process, spawning a new one with a different
// memory space.
uint32_t fork();

// Causes the current process' stack to be forcibly moved to a new location.
void move_stack(void *new_stack_start, uint32_t size);

// Returns the pid of the current process.
uint32_t getpid();

#endif
