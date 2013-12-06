#include <task.h>
#include <scheduler.h>
#include <stdio.h>

void kexit(int status) {
  current_task->exit_status = status;
  current_task->STATUS = TASK_ZOMBIE;
  if (DEBUG) printf("Exiting with status %d\n", status);
  switch_task();  
  printf("Warning: Should never come here at the end of kexit.c");
  return;
}
