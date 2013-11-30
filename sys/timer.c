#include<stdio.h>
#include<pic.h>
#include<idt.h>
#include<utilities.h>
#include<idt.h>
#include <task.h>

extern volatile task_t* current_task;
extern volatile task_t* ready_queue;

extern void switch_task();


void timer_handler(struct regs *r)
{
    static int timer_ticks = 0;

    timer_ticks++;
    if ((timer_ticks-1) % 18 == 0) {
        print_time(timer_ticks);
//        printf("Num tasks: %d\n", numtasks());
        if (current_task) {
         // printf("Switching from timer\n");
          switch_task();
          //printf("Back from switch task\n");
        }
    }
}

