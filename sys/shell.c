#include <stdio.h>
#include <task.h>

void create_shell() {
    //printf("Execing");
    printf("Creating deep shell\n");
    char *argv[] = {(char*)NULL};
    kexecvpe("bin/dsh", 0, argv, argv);
    printf("WARNING: Back after exec of shell");
    return;
}

