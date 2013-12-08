#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
   int p12 = fork();
   char *idle[2] = {"/bin/idle", NULL};
    if (p12 == 0)
      execvpe(idle[0], idle, idle);
   char *dsh[2] = {"/bin/dsh", NULL};
   execvpe(dsh[0], dsh, envp);
}
