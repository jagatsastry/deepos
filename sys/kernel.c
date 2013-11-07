#include<stdio.h>
#include<idt.h>
#include<utilities.h>
#include<pic.h>
#include <mem.h>

void init_kernel(void* modulep, void* physbase, void* physfree) {
    init_pics();
    idtStart();
    init_phys_mem(modulep, physbase, physfree);
    printf("Booting Deep-OS");
}

