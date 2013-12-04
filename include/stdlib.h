#ifndef _STDLIB_H
#define _STDLIB_H
#include <defs.h>

void exit(int status);
void memset(void *init, char val, int sz);
//assume no overlap

void memcpy(void *dest, void *src, uint64_t num_bytes);

int numtasks();
uint32_t getpid();
uint32_t fork();
int exec(char* filename) ;

void exit(int status);
void sleep(uint32_t time);
uint32_t waitpid(uint32_t pid, uint32_t *status);
uint32_t wait(uint32_t *status);
void *sbrk();
pid_t getpid();

#endif
