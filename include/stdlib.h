#ifndef _STDLIB_H
#define _STDLIB_H
#include <defs.h>

void exit(int status);
void memset(void *init, char val, int sz);
//assume no overlap

void memcpy(void *dest, void *src, uint64_t num_bytes);

pid_t numtasks();
uint32_t getpid();
uint32_t fork();
int exec(char* filename) ;
int execvpe(char* filename, char *argv[], char *argp[]) ;

void exit(int status);
void sleep(uint32_t time);
pid_t waitpid(pid_t pid, uint32_t *status, int options);
pid_t wait(int *status);
void *sbrk();
pid_t getpid();

#endif
