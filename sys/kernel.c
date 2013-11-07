#include<stdio.h>
#include<idt.h>
#include<utilities.h>
#include<pic.h>
#include <mem.h>

void init_kernel(void* modulep, void* kernmem, void* physbase, void* physfree) {
    init_pics();
    idtStart();
    printf("xx %x %x \n", (uint64_t)physbase, (uint64_t)physfree);
    init_phys_mem(modulep, kernmem, physbase, physfree);
    printf("Booting Deep-OS");
}

