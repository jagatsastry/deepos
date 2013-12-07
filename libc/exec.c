#include <defs.h>
#include <stdio.h>

int execvpe(const char* filename, char *const argv[], char *const argp[]) {
    if (DEBUG) printf("Running execvpe of %s\n", filename);
    if (DEBUG) printf("In execvpe: Arguments:  %x %x\n",  argv, argp);
    if (DEBUG) printf("In execvpe: Arguments:  %s\n",  argv[0]);
    
    pid_t ret = -1;
    __asm__ __volatile__ (
              "movq $0x5, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               movq %2, %%rsi;\
               movq %3, %%rdi;\
               int $0x80;\
               ":: "g"((uint64_t)filename), "g"((uint64_t)argv), "g"((uint64_t)argp), "g"((uint64_t)&ret) :"rbx","rdx", "rcx", "rsi", "rdi", "memory");
    if (DEBUG) printf("In execvpe ret val: %d\n", ret);
    return ret;
}

int exec(const char *filename) {
  char *const argv[] = {(char *const)filename, NULL};
  return execvpe(filename, argv, argv);
}
