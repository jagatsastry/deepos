#include <task.h>
#include <string.h>
#include <stdio.h>
#include <defs.h>

uint64_t get_mem_usage(task_t *task) {
  uint64_t mem = 0;
  for (int i = 0; i < VMA_DEF_COUNT; i++) {
    if (task->vma[i].start_addr && task->vma[i].end_addr) 
      mem += (task->vma[i].end_addr - task->vma[i].start_addr);
  }
  return mem;
}

extern char *itoa(unsigned long int num);

void printTask(task_t *task, char *options) {
  printf("%s%s%s%s\n", ralign(3, itoa(task->id)), ralign(7, itoa(task->run_time)), 
  ralign(10, itoa(get_mem_usage(task))), ralign(17, (char*)task->program_name));
}

void kps(char *options) {

    printf("PID  TIME(ms)  MEM(kb)       CMD\n");
    for (int i = 0; i < 100; i++) {
      if (ready_queue[i].STATUS != TASK_ZOMBIE && ready_queue[i].STATUS != TASK_FREE)
        printTask(&ready_queue[i], options);
    }
}
