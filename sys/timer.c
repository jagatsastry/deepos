#include<stdio.h>
#include<pic.h>
#include<idt.h>
#include<utilities.h>
#include<idt.h>
#include <task.h>

extern task_t* current_task;

void timer_handler(struct regs *r)
{
    static int timer_ticks = 0;

    if (timer_ticks % 18 == 0) {
        print_time(timer_ticks);
        if (current_task) {
          printf("Switching from timer\n");
          switch_task();
          printf("Back from switch task\n");
        }
    }
    timer_ticks++;
}

