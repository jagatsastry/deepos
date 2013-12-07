#include <main.h>
#include <stdlib.h>
#include <stdio.h>

void _start(int argc, char* argv[], char* envp[]) {
  if (DEBUG) printf("In start arguments: %d %x %x\n", argc, argv, envp);
	int res = main(argc, argv, envp);
	exit(res);
}
