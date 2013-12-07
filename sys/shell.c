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
    char *argv[] = {(char*)filename, NULL};
    kexecvpe("bin/init_sh", 1, argv, argv);
    printf("WARNING: Back after exec of shell");
    return;
}

