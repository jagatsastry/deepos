#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  printf("WHATEVER *********Sleeping********\n");
  sleep(1);
  printf("WHATEVER Back from sleeping: PID %d\n", getpid());
  pid_t id = getpid();
  
  printf("WHATEVER HAHAHA PID: %d\n", id);
  int pid = fork();
  if (pid != 0) {
    uint32_t status;
    printf("WHATEVER Waiting for child %d\n", pid);
    int pid1 = waitpid(pid, &status, 0);
    printf("WHATEVER Child %d supposedly exited with status %d\n", pid1, status);
  }
  //printf("WHATEVER Forking a new process in %d\n", getpid());
  //int p1 = fork();
  //printf("WHATEVER %d is back in hello. Fork returned %d!\n", getpid(), p1);
  printf("WHATEVER %d is back in hello. !\n", getpid());
  
  //printf("WHATEVER Back from stuff\n");
  //while(1);
  //printf("WHATEVER Enter stuff\n");
  //char stuff[10];
  //scanf("WHATEVER %s", stuff);
	//printf("WHATEVER Hello %s!\n", stuff);
  exit(1);
}
