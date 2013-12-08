#include <stdio.h>
#include <task.h>
#include <string.h>
#include <utilities.h>

extern void clear_line(int);

void create_shell() {
    //printf("Execing");
    printf("Creating deep shell\n");
    char *fnm = "bin/init_sh";
    char *filename = (char*)kmalloc(strlen(fnm) + 1);
    strcpy(filename, fnm); 

    char **envp = kmalloc(64 * sizeof(char*));
    for (int i = 0; i < 64; i++)
      envp[i] = NULL;
    envp[0] = kmalloc(128);
    strcpy(envp[0], "PATH=bin");

    char **argv = kmalloc(64 * sizeof(char*));
    argv[0] = filename;
    kexecvpe("/bin/init_sh", 1, argv, envp);
    printf("WARNING: Back after exec of shell");
    return;
}
