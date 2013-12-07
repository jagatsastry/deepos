#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_malloc() {
  printf("Testing malloc\n");
  char *str = "string on heap\n";
  for (int i = 0; i < 1; i++) {
    char *test_str = (char*)malloc(20);
    strcpy(test_str, str);
    printf("Following str starts at %x\n", (uint64_t)test_str);
    printf(test_str);
  }
}
  
void test_sleep() {
  printf("*********Sleeping******** for 2 sec\n");
  sleep(2);
  printf("Back from sleeping: PID %d\n", getpid());
}

int main(int argc, char* argv[]) {
  pid_t id = getpid();
  test_malloc();
  test_sleep();
  
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
