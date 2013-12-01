#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  printf("*********Sleeping********\n");
  sleep(2);
  int pid = fork();
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
