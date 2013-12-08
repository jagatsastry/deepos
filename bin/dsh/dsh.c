#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setenv(char **envp, char *var, char *val) {
  int i = 0;
  for (; i < 64 && envp[i]; i++) {
    if (startsWith(envp[i], var)) {
      strcpy(envp[i], strcat(strcat(var, "="), val));
      return;
    }
  }
  envp[i] = malloc(128);
  strcpy(envp[i], strcat(strcat(var, "="), val));
}

void printEnv(char **envp) {
    //printf("Env\n");

    for (int i = 0; i < 64 && envp[i]; i++) {
      printf("%s\n", envp[i]);
    }
}

    
char * strtok(char * str, char *comp);

void dsh(int argc, char *argv[], char *envp[]) {
   strcpy(envp[0], "PATH=/bin");
   envp[1] = NULL;

   if (DEBUG) printf("Just entered dsh: PID %d\n", getpid());
   for( ; ; ){ 
       char* shellPrompt = strcat(strcat("dsh [", pwd()), "]$ "); 
       printf("\n%s",shellPrompt);
       
       char *command = malloc(256); //[ 256 ] = { 0 };
       int numBytes = scanf("%s", command );   
       command = trim(command);

      if( command[0] == '\0'){ 
          if (DEBUG) printf("enterPressed");  
          continue;
       }
       if( numBytes < 0 ){
           printf("\nShell Error");
       }
       if (DEBUG) printf("\nCommand entered is %s",command);  
       volatile int bg = command[strlen(command) - 1] == '&';
       if (bg)
         command[strlen(command) - 1] = 0;

       argc = 0;
       argv[ argc ] = strtok( command, " \t\n"); 
       while( argc++ < 64 )
           if( !(argv[ argc ] = strtok( NULL, " \t\n" )) ) break;
       for(int i = 0; i < argc;i++){
           if (DEBUG) printf("\n%s", argv[i]);
       }
       if (DEBUG) printf("argc %d\n", argc);
       if (!strcmp(argv[0], "setenv")) {
         if (argc != 3) {
           printf("Usage: setenv <ENV_VAR> <VAL>");
           continue;
         }
         setenv(envp, argv[1], argv[2]);
         continue;
       }
       if (!strcmp(command, "env")) {
         printEnv(envp);
         continue;
       }
       if (!strcmp(argv[0], "cd")) {
         if (argc <= 1 || argc > 2) {
           printf("Usage: cd <dir>");
           continue;
         }
         cd(argv[1]);
         continue;
       }
       if (!strcmp(argv[0], "sh")) {
         if (argc > 1) {
           printf("Usage: sh");
           continue;
         }
         strcpy(argv[1], "dsh");
         continue;
       }

       int pid = fork();
       if (pid == 0) {
           if(execvpe(argv[0], argv, envp) < 0){
             //TODO: There's a bug where any subsequent command fails
               printf("dsh: %s: command not found\n", command);
               exit(1);
           }
       } else  {
           int status = 0;
           if (!bg) {
             //waitpid(pid, (uint32_t*)&status, 0); 
             waitpid(pid, &status, 0); 
             //wait(&status);
           } else
            printf("[bg]: %d\n", pid);
            bg = 0;
           if (DEBUG) printf("Returned from a long wait\n");
       }
   }
}


int main(int argc, char *argv[], char *envp[]) {
  dsh(argc, argv, envp);
  return 0;
}

