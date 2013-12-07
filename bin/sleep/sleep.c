#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
  if (DEBUG) printf("In sleep\n");
  if (argc < 2) {
    printf("Usage: sleep <time in sec>\n");
    return 1;
  }

  sleep(atoi(argv[1]));
}
