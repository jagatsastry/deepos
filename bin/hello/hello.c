#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  printf("*********Sleeping********\n");
  sleep(1);
  pid_t id = getpid();
  
  printf("HAHAHA PID: %d\n", id);
  int pid = fork();
  if (pid != 0) {
    uint32_t status;
    printf("Waiting for child %d\n", pid);
    int pid1 = wait(&status);
    printf("Child %d supposedly exited with status %d\n", pid1, status);
  }
  printf("Back from sleeping: PID %d\n", pid);
  //printf("Back from stuff\n");
  //while(1);
  //printf("Enter stuff\n");
  //char stuff[10];
  //scanf("%s", stuff);
	//printf("Hello %s!\n", stuff);
  printf(" 1 Back from sleeping: PID %d\n", pid);
  exit(1);
}
