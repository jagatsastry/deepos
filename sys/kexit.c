#include <task.h>
#include <scheduler.h>
#include <stdio.h>

void kexit(int status) {
  current_task->exit_status = status;
  current_task->STATUS = TASK_ZOMBIE;
  printf("Exiting with status %d\n", status);
  switch_task();  
  printf("Should never come here");
  return;
}
