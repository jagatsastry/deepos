#include <defs.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[], char *argp[]) {
  if (argc <= 1 || (strcmp(argv[1], "Sv") != 0 &&  strcmp(argv[1], "Hv") != 0)) {
    printf("Usage: ulimit -[H/S]v <memory in kb>");
    return 1;
  }
  size_t lim = atoi(argv[1]);
  int taskCode = 1;
  if (lim == 0) {
    if (DEBUG) printf("0 limit. noop\n");
    return 0;
  } else {
    __asm__( "movq $16, %%rbx;\
             movq %0, %%rdx;\
             movq %1, %%rcx;\
             int $0x80;\
             "::"g" ((uint64_t)lim), "g"((uint64_t)taskCode):"rbx", "rcx", "rdx") ;
  }
    return 0;
}
