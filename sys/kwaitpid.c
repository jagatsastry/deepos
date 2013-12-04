#include <task.h>
#include <timer.h>
#include <scheduler.h>
#include <stdio.h>

pid_t kwaitpid(pid_t pid, uint32_t *status) {
  current_task->STATUS = TASK_WAITING_ON_PID;
  current_task->pid_waiting_for = pid;
    
  //__asm__ __volatile__("movq %%rsp, %0" : "=r"(current_task->rsp));
  //Simple trick: We have set the rip and rsp. 
  //When we return from switch_task after a switch, we unset the RIP, so we know that it is a jump from timer.
  printf("%d waiting on %d\n", current_task->id, (int)pid);
  switch_task();
  *status = get_task(pid)->waiting_pid_exit_status;
  pid = current_task->pid_waiting_for;
  printf("%d done waiting on %d\n", current_task->id, pid);
  return pid;
}

