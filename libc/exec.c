#include <defs.h>
#include <stdio.h>

int execvpe(const char* filename, char *const argv[], char *const argp[]) {
  printf("Running execvpe of %s\n", filename);
    pid_t ret = -1;
    __asm__ __volatile__ (
              "movq $0x5, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               movq %2, %%rsi;\
               movq %3, %%rdi;\
               int $0x80;\
               ":: "g"((uint64_t)filename), "g"((uint64_t)argv), "g"((uint64_t)argp), "g"((uint64_t)&ret) :"rbx","rdx", "rcx", "rsi", "rdi", "memory");
    while(1);
    printf("Ret val: %d\n", ret);
    return ret;
}

int exec(const char *filename) {
  char *const argv[] = {NULL};
  return execvpe(filename, argv, argv);
}
