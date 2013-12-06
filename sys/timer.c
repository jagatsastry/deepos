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
        print_time(timer_ticks);
//        if (DEBUG) printf("Num tasks: %d\n", numtasks());
        if (current_task) {
          updateKeyPressEvent();
         // if (DEBUG) printf("Switching from timer\n");
          update_waiting_and_sleeping_tasks();
          //if (DEBUG) printf("Switching task\n");
          //kill_zombies(); //Braiiiiinssss
          //if (current_task->id == 2 && timer_ticks > TICK_PER_SECOND + 2)
           // kexit(30);
         // else 
         if (DEBUG)   printf("Calling switch task from timer interrup\n");
            switch_task();
          //if (DEBUG) printf("Back from switch task\n");
        }
    }
}

