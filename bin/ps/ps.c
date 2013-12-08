#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv) {
  char *options = NULL;
  if (argc > 1)
    options = argv[1];
  ps(options);
  return 0;
}
