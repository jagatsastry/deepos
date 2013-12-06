#include<stdio.h>
#include<pic.h>
#include<idt.h>
#include<utilities.h>
#include<idt.h>
#include <task.h>
#include <timer.h>
#include <scheduler.h>

void updateKeyPressEvent() {
    if(enterPressed == 1 && current_task->waiting_for_input == 1) {
      current_task->waiting_for_input = 0;
      enterPressed = 0;
      *keyPressedPtr = 1;
    }
}

void timer_handler(struct regs *r)
{
    static int timer_ticks = 0;

    timer_ticks++;
    if ((timer_ticks-1) % TICK_PER_SECOND == 0) {
        print_status_bar(timer_ticks);
        if (current_task) {
          updateKeyPressEvent();
          update_waiting_and_sleeping_tasks();
         if (DEBUG == 2)   printf("Calling switch task from timer interrup\n");
            switch_task();
        }
    }
}

