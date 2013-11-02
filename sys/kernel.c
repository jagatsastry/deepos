#include<stdio.h>
#include<idt.h>
#include<utilities.h>
#include<pic.h>

void init_kernel(void* modulep, void* physbase, void* physfree) {
    init_pics();
    idtStart();
    printf("Booting Deep-OS");
}

