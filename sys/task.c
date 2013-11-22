#include <pt.h>
#include <defs.h>
#include <stdio.h>
#include <sys/gdt.h>

extern void _enter_user_mode();
extern struct tss_t tss;

void enter_user_mode () {
  printf("Entering user mode\n");
  tss.rsp0 = cpu_read_rsp();
  printf("%x\n", tss.rsp0);
  _enter_user_mode();  
  printf("In user mode\n");
  while(1);
}
