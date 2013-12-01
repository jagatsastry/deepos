#include<stdio.h>
#include<pic.h>
#include<idt.h>
#include<utilities.h>
#include<idt.h>
#include <task.h>
#include <timer.h>
#include <scheduler.h>



void timer_handler(struct regs *r)
{
    static int timer_ticks = 0;

    timer_ticks++;
    if ((timer_ticks-1) % TICK_PER_SECOND == 0) {
        print_time(timer_ticks);
//        printf("Num tasks: %d\n", numtasks());
        if (current_task) {
         // printf("Switching from timer\n");
          update_waiting_and_sleeping_tasks();
          //kill_zombies(); //Braiiiiinssss
          //if (current_task->id == 2 && timer_ticks > TICK_PER_SECOND + 2)
           // kexit(30);
         // else 
            switch_task();
          //printf("Back from switch task\n");
        }
    }
}

