
void create_shell() {
  int pid = kfork_wrapper(1); 
  if (pid == 0) {
    //printf("Execing");
    printf("Creating deep shell\n");
    char *argv[] = {(char*)NULL};
    kexecvpe_wrapper("bin/dsh", 0, argv, argv, 1);
    printf("WARNING: Back after exec");
    return;
  }
  printf("Back in kernel after invoking shell\n");
  return;

}

