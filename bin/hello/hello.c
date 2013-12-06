#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  printf("*********Sleeping********\n");
  sleep(2);
  printf("Back from sleeping: PID %d\n", getpid());
  pid_t id = getpid();
  
  printf("HAHAHA PID: %d\n", id);
  /*
  int pid = fork();
  if (pid != 0) {
    uint32_t status;
    printf("Waiting for child %d\n", pid);
    int pid1 = waitpid(pid, &status, 0);
    printf("Child %d supposedly exited with status %d\n", pid1, status);
  } else {
    printf("Execing WHATEVER\n");
    exec("bin/whatever");
    printf("Back from exec. SHOULDNT HAVE\n");
  }
  //printf("Forking a new process in %d\n", getpid());
  //int p1 = fork();
  //printf("%d is back in hello. Fork returned %d!\n", getpid(), p1);
  printf("%d is back in hello. !\n", getpid());
  
  //printf("Back from stuff\n");
  //while(1);
  //printf("Enter stuff\n");
  //char stuff[10];
  //scanf("%s", stuff);
	//printf("Hello %s!\n", stuff);
  exit(1);
  */
}
