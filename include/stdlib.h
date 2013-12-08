#ifndef _STDLIB_H
#define _STDLIB_H
#include <defs.h>

void exit(int status);
void memset(void *init, char val, int sz);
//assume no overlap

void memcpy(void *dest, void *src, uint64_t num_bytes);

pid_tt numtasks();
uint32_t getpid();
uint32_t fork();
int exec(char* filename) ;
int execvpe(char* filename, char *argv[], char *argp[]) ;
int execve(const char* filename, char *const argv[]);

void exit(int status);
void sleep(uint32_t time);
pid_tt waitpid(pid_tt pid, int *status, int options);
pid_tt wait(int *status);
void *sbrk();
pid_tt getpid();
void* malloc(size_tt);
int atoi(char *p);
char *pwd();
int cd(char*);
void ps(char *options);

#endif
