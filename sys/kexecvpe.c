#include<stdio.h>
#include <string.h>
#include<defs.h>
#include <pic.h>
#include<sys/tarfs.h>
#include<elf.h>
#include<phy_mem.h>
#include<virt_mem.h>
#include <task.h>
#include <sys/gdt.h>


extern void* i_virt_alloc();
extern void print_current_task();
extern uint64_t i_virt_to_phy(uint64_t virt);

char* addSlash(char *curPath) {
      if (curPath[strlen(curPath) - 1] !=  '/')
        curPath = strcat(curPath, "/");
      return curPath;
}

char** getPath(char **envp) {
  char **path = kmalloc(64 * sizeof(char*));

  int ct = 0;
  for (int i = 0;  envp && envp[i]; i++) {
    if (startsWith(envp[i], "PATH=")) {
      char *pathStr = envp[i] + 5;

      path[ct] = strtok(pathStr, ";");
      path[ct] = addSlash(path[ct]);

      while( ct++ < 63 ) {
           if( !(path[ct] = strtok(NULL, ";" )) ) break;
           path[ct] = addSlash(path[ct]);
      }
      for(int j = 0; j < ct;j++){
         if (DEBUG) printf("\nPATH %d: %s", i, path[i]);
      }
    }
  }
  path[ct] = NULL;
  return path;
}

int kexecvpe(char* filename, int argc, char *argv[], char *envp[]) {
  current_task->argc = argc;
  int i = 0;
  for (; argv && argv[i]; i++) {
    if (DEBUG) printf("execve: %d \n", i);
    if (DEBUG) printf("execve: Copying argv %d %s\n", i, argv[i]);
    current_task->argv[i] = (char*)kmalloc(strlen(argv[i]) + 1);
    strcpy(current_task->argv[i], argv[i]);
  }
  current_task->argv[i] = NULL;
  i = 0;
  for (;  envp && envp[i]; i++) {
    if (DEBUG) printf("execve: Copying envp %d %s\n", i, envp[i]);
    current_task->envp[i] = (char*)kmalloc(strlen(envp[i]) + 1);
    strcpy(current_task->envp[i], envp[i]);
  }
  current_task->envp[i] = NULL;

  if (DEBUG) printf("Running kexecvpe of %s\n", filename);
  struct Exe_Format exeFormat;
  uint64_t elf_start = 0;
  struct  posix_header_ustar *tar_p = get_elf_file(filename, &exeFormat, &elf_start);
  if (tar_p == NULL) {
    if (DEBUG) printf("%s not found. Searching in PATH\n", filename); 
    char ** path = getPath(envp);
    char *filename1 = filename;
    for (int i = 0; path && path[i]; i++) {
      filename = strcat(path[i], filename1);
      if (DEBUG) printf("Searching %s\n", filename);
      tar_p = get_elf_file(filename, &exeFormat, &elf_start);
    }
//    if(tar_p != NULL) printf("cmd: %s\n", 
  }
  if (tar_p == NULL) {
    if (DEBUG) printf ("Going back from kexecve\n");
    return -1;
  }
  print_posix_header(tar_p);
  char* x = tar_p->size;
  if (DEBUG) printf("\n%s\n", x);
  if (DEBUG) printf("\n%s\n", x);
  if (DEBUG) printf("hi\n");
  for (i = 0; i < 10; i++)
    current_task->vma[i].start_addr = 0;
  current_task->vma[VMA_USER_STACK_IDX].start_addr = (uint64_t)i_virt_alloc(); //User stack
  current_task->vma[VMA_KERNEL_STACK_IDX].start_addr = (uint64_t)i_virt_alloc(); //Kernel stack
  
  for (i = 0; i < VMA_SEGMENT_START; i++)
    current_task->vma[i].end_addr = current_task->vma[i].start_addr + 4095;

  //strcpy((char*)current_task->temp[0], filename);
  map_exe_format(&exeFormat, &elf_start);
  if (DEBUG) printf("Preparing the stack\n");
  current_task->execEntryAddress = exeFormat.entryAddr;
  current_task->u_rsp = (uint64_t)current_task->vma[VMA_USER_STACK_IDX].end_addr;
  current_task->rsp = (uint64_t)current_task->vma[VMA_KERNEL_STACK_IDX].end_addr;
  if (DEBUG) printf("In execve PID: %d. USP %x RSP %x\n", current_task->id, current_task->u_rsp, current_task->rsp);
  current_task->tss_rsp = (uint64_t)current_task->rsp;
  tss.rsp0 = (uint64_t)current_task->rsp;
  if (DEBUG) printf("Move the stack temporarily\n");
  //strcpy((char*)current_task->program_name, current_task->temp[0]);
  print_current_task();

  uint64_t phy_pml4e = i_virt_to_phy((uint64_t)current_task->pml4e);

  __asm__ __volatile__("movq %0, %%cr3" : : "r" (phy_pml4e));
  port_outb(0xA0, 0x20);
  port_outb(0x20, 0x20);
  __asm__ __volatile__( "movq %0, %%rsp ": : "m"(current_task->rsp) : "memory");
  __asm__ __volatile__("pushq $0x23\n\t"
                 "pushq %0\n\t"
                 "pushq $0x200292\n\t"
                 "pushq $0x1b\n\t"
                 "pushq %1\n\t"
       : :"c"(current_task->u_rsp),"d"((uint64_t)current_task->execEntryAddress) :"memory");

  current_task->run_sessions_count++;
  current_task->run_time += SCHEDULE_FREQUENCY;

  if (DEBUG) printf("In execve: argc %d argv[0] %s\n", argc, current_task->argv[0]);
  if (argc > 1)
    if (DEBUG) printf("In execve: argc %d argv[1] %s\n", argc, current_task->argv[1]);

  __asm__ __volatile__("movq %0, %%rdi\n;"
            "movq %1, %%rsi\n;"
            "movq %2, %%rdx\n;" :: 
            "g"((uint64_t)current_task->argc), "g"((uint64_t)current_task->argv), "g"((uint64_t)current_task->envp) :
            "rdi", "rsi", "rdx", "memory");
  __asm__ __volatile__ ("iretq":::"memory");
  return 0;
}
