#include <task.h>
#include <timer.h>
#include <stdio.h>

extern uint64_t _read_rip();

void ksleep(uint32_t time) {
  current_task->sleeping_time = time * 18;
  current_task->STATUS = TASK_SLEEPING;

  if (DEBUG) printf("%d sleeping for %d seconds\n", current_task->id, time);
  switch_task();
  if (DEBUG) printf("I am back from sleep. pid: %d\n", current_task->id);
}
