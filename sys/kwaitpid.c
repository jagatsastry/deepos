#include <task.h>
#include <timer.h>
#include <scheduler.h>
#include <stdio.h>

uint32_t kwaitpid(uint32_t pid, uint32_t *status) {
  current_task->STATUS = TASK_WAITING_ON_PID;
  current_task->pid_waiting_for = pid;
    
  //__asm__ __volatile__("movq %%rsp, %0" : "=r"(current_task->rsp));
  //Simple trick: We have set the rip and rsp. 
  //When we return from switch_task after a switch, we unset the RIP, so we know that it is a jump from timer.
  printf("%d waiting on %d\n", current_task->id, pid);
  switch_task();
  *status = get_task(pid)->waiting_pid_exit_status;
  printf("%d done waiting on %d\n", current_task->id, pid);
  return pid;
}

uint32_t kwait(uint32_t *status) {
  int i = 0;
  printf("In kwait\n");
  for (i = 0; i < MAX_TASKS; i++) {
    task_t *task = &ready_queue[i];
    if (task->parent == current_task)
      return kwaitpid(task->id, status);
  }
  return -1;
}
