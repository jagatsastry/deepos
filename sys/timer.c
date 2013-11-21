#include<stdio.h>
#include<pic.h>
#include<idt.h>
#include<utilities.h>
#include<idt.h>


void timer_handler(struct regs *r)
{
    printf("In timer handler\n");
    static int timer_ticks = 0;

    if (timer_ticks % 18 == 0) {
        print_time(timer_ticks);
    }
    timer_ticks++;
}

