#include <stdio.h>
#include <task.h>
#include <utilities.h>

extern void clear_line(int);

void create_shell() {
    //printf("Execing");
    printf("Creating deep shell\n");
    char *argv[] = {(char*)NULL};
    kexecvpe("bin/init_sh", 0, argv, argv);
    printf("WARNING: Back after exec of shell");
    return;
}

