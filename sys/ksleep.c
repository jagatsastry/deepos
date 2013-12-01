#include <task.h>
#include <timer.h>
#include <stdio.h>

extern uint64_t _read_rip();

void ksleep(uint32_t time) {
  current_task->sleeping_time = time * TICK_PER_SECOND;
  current_task->STATUS = TASK_SLEEPING;

  //__asm__ __volatile__("movq %%rsp, %0" : "=r"(current_task->rsp));
  //Simple trick: We have set the rip and rsp. 
  //When we return from switch_task after a switch, we unset the RIP, so we know that it is a jump from timer.
  printf("%d sleeping for %d seconds\n", current_task->id, time);
  switch_task();
  printf("I am back from sleep. pid: %d\n", current_task->id);
}
