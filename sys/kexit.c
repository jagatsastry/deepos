#include <task.h>
#include <scheduler.h>
#include <stdio.h>
#include <timer.h>

void kexit(int status) {
  current_task->exit_status = status;
  current_task->STATUS = TASK_ZOMBIE;
  current_task->end_time = timer_tick;
  //current_task->parent = 0; //A zombie has no parent
  if (DEBUG) printf("Exiting with status %d\n", status);
  switch_task();  
  printf("Warning: Should never come here at the end of kexit.c");
  return;
}
