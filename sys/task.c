#include <defs.h>
#include <stdio.h>
#include <sys/gdt.h>

extern void _enter_user_mode();
extern struct tss_t tss;

typedef uint32_t taskid_t;

extern uint64_t virtual_address_k;

void enter_user_mode () {
  printf("Entering user mode\n");
  tss.rsp0 = virtual_address_k;
  printf("Stack addr: %x\n", tss.rsp0);
  _enter_user_mode();  
  printf("In user mode\n");
  while(1);
}

/*
struct Task {
  struct Task *nexttask;
  uint8_t waiting;
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rbp;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t rsp;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rflags;
  uint16_t ds;
  uint16_t es;
  uint16_t fs;
  uint16_t gs;
  uint16_t ss;
  uint64_t cr3;
  uint64_t firstdata;
  uint64_t firstfreemem;
  uint64_t nextpage;
  uint16_t pid;
  uint64_t timer;
  char *environment;
  struct MessagePort *parentPort;
  char *currentDirName;
  char **argv;
  uint64_t console;
  uint8_t forking;
//  struct FCB *fcbList;
};

taskid_t kfork()
{
  // Copy task structure, with adjustments
  struct Task *task = (struct Task *)alloc_virt_above_kernel());
  copyMem((uint8_t *) currentTask, (uint8_t *) task,
      sizeof(struct Task));
  int pid = task->pid = nextpid++;
  task->currentDirName = AllocKMem(strlen(currentTask->currentDirName) + 1);
  strcpy(task->currentDirName, currentTask->currentDirName);
  task->parentPort = 0;

  // Copy Page Table and pages
  task->cr3 = (uint64_t) VCreatePageDir(pid, currentTask->pid);
  task->forking = 1;

  // Create FCBs for STDI, STDOUT, and STDERR

  // STDIN
  struct FCB *fcbin = (struct FCB *) AllocKMem(sizeof(struct FCB));
  fcbin->fileDescriptor = STDIN;
  fcbin->deviceType = KBD;
  task->fcbList = fcbin;

  // STDOUT
  struct FCB *fcbout = (struct FCB *) AllocKMem(sizeof(struct FCB));
  fcbout->fileDescriptor = STDOUT;
  fcbout->deviceType = CONS;
  fcbin->nextFCB = fcbout;
  ;

  //STDERR
  struct FCB *fcberr = (struct FCB *) AllocKMem(sizeof(struct FCB));
  fcberr->fileDescriptor = STDERR;
  fcberr->deviceType = CONS;
  fcbout->nextFCB = fcberr;
  fcberr->nextFCB = 0;

  // Run the forked process
  asm("cli");
  LinkTask(task);
  asm("sti");

  // We want the forked process to return to this point. So we
  // need to save the registers from here to the new task structure.
  SaveRegisters(task);

  // Return 0 to the forked process, the new pid to the forking one.
  if (pid == currentTask->pid)
    pid = 0;
  return pid;
}
*/
