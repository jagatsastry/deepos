#include <stdio.h>
#include <pic.h>
#include <defs.h>
#include <sys/tarfs.h>
#include <idt.h>
#include <string.h>
#include <task.h>
#include <files.h>
#include <syscall.h>


void print( struct regsForSyscall *regs)
{
    char *strPtr = (char *)regs->rdx;
    int i=0;
    if (DEBUG) printf("%p",strPtr);
    while( strPtr[i] != '\0' )
    {
           putc(strPtr[i]);
           i++; 
    }
}

void exitSyscall(struct regsForSyscall *regs)
{
    kexit(*(uint64_t*)regs->rdx);
}


//void enterToMemory( void *Addr, int typeOfArg)
void enterToMemory( struct regsForSyscall *regs) 
{
      
     void *Addr = (void *)regs->rax;
     int typeOfArg = (int )regs->rcx; 
     if( typeOfArg == 1)
     {
           int *memAddr = (int *)Addr;
           int buffIndex = 0;
           int num = 0;
           while( buffIndex < curBuffIndex )
           {
               num = num * 10 + ( buff[ buffIndex ] - '0');
               buffIndex++;
           }
           *memAddr = num;
     }
     if( typeOfArg == 2){
           char *memAddr = (char *)Addr;
           int buffIndex = 0;
           *memAddr = buff[buffIndex];
     }
     if( typeOfArg == 3){ 
           char *memAddr = (char *)Addr;
           int buffIndex = 0;
           while( buffIndex < curBuffIndex ){//!(buff[ buffIndex ] == 'r')){ 
              memAddr[buffIndex] = buff[buffIndex]; 
              buffIndex++;
           }
     }
}


void sscanfSystemCall( struct regsForSyscall *regs )
{
    //const char * formatStr = ( const char *)regs->rdx;
    //void* memAddr = (void *)regs->rdx;
    //if (DEBUG) printf("\nAlok %p",regs->rdx); 
    //if (DEBUG) printf("\nAbhishek %p",regs->rcx);
    keyPressedPtr = (int *)regs->rcx; 
    current_task->waiting_for_input = 1;
    enterPressed = 0;
    curBuffIndex = 0;
    //if (DEBUG) printf("\nValue of keyPressedPtr is %p",keyPressedPtr); 
    //if (DEBUG) printf("The sting is %x",buff);
    //enterToMemory(memAddr,1);

}




extern void cpu_write_rcx();

void sys_sleep(struct regsForSyscall * s) {
  if (DEBUG) printf("Sleeping for %ds\n", s->rdx);
  ksleep(*(uint64_t*)s->rdx);
}

void sys_waitpid(struct regsForSyscall * s) {
  pid_t pid = (pid_t)s->rdx;
  pid_t *retPid = (uint32_t*)s->rsi;
  uint32_t *status =  *(uint32_t**)s->rcx;
  *retPid = kwaitpid(pid, status);
}


void sys_fork(struct regsForSyscall * s) {
  uint64_t* rval = (uint64_t*)s->rdx;
  int ret = kfork(s);
  *rval = ret;
  if (DEBUG) printf("Returning from fork\n");
}

extern void* i_virt_alloc();

void sys_sbrk(struct regsForSyscall * s) {
  if (current_task->current_mem_usage + 4096 > current_task->mem_limit) {
    printf("Err: Memory limit exceeded for PID %d\n", current_task->id);
    return;
  }
  uint64_t addr = (uint64_t)i_virt_alloc();
  int i = 0;
  for (; i < 10; i++) {
    if (current_task->vma[i].start_addr == NULL) {
      current_task->vma[i].start_addr = addr;
      current_task->vma[i].end_addr = addr + 4095;
      printf("PID: %d called sbrk. Inserting %x-%x into vma slot %d\n", 
        current_task->id, current_task->vma[i].start_addr, 
        current_task->vma[i].start_addr, i);
      break;
    }
  }

  current_task->current_mem_usage += 4096;
  *(uint64_t*)s->rdx = addr;
}

void sys_getpid(struct regsForSyscall * s) {
  *(uint64_t*)s->rdx = current_task->id;
}

void sys_execvpe(struct regsForSyscall * s) {
  char *filename = (char*)s->rdx;
  char **argv = (char**)s->rcx;
  char **argp = (char**)s->rsi;
  int *ret = (int*)s->rsi;

  if (DEBUG) printf("syscall: Running execvpe of %s\n", filename);
  int argc = 0;
  for(; argv[argc]; argc++);

  *ret = kexecvpe(filename, argc, argv, argp);
  if (DEBUG) printf("In syscall-execvpe. Going back\n");
}

int fdAllocArray[100] = {0};
int curLocation = 0;
int dirDesc[ 100 ] = {0};

void sys_open( struct regsForSyscall *regs )
{
   char *fname = (char *)regs->rax; 
   int *retVal = (int *)regs->rcx; 
   uint64_t start = (uint64_t)&_binary_tarfs_start;
   uint64_t end = (uint64_t)&_binary_tarfs_end;
   struct posix_header_ustar *ptr;
   int  isPresent = 0, size ;
   ptr = (struct posix_header_ustar *)start;

   while( start < end  ){
       ptr = (struct posix_header_ustar *)start;
       if( !strcmp( ptr->name,fname)){
            start = start + sizeof(struct posix_header_ustar );
            size = 0; int i = 0;
            while( ptr->size[i] != '\0'){
                size = size*10 + ( ptr->size[i] - '0');
                i++ ;
            }
            if( size == 0 )
            {
                *retVal = -1;
            } 
            size = octal_decimal( size );
            isPresent = 1;
            break;
       }

       size = 0; int i =0;
       while( ptr->size[i] != '\0'){
           size = size*10 + ( ptr->size[i] - '0');
           i++ ;
       }
       size = octal_decimal( size );
       int offset = 0;
       if( size % 512 != 0 )
       {
           offset =   512 - size % 512;
       }

       //t++;
       start = start + sizeof(struct posix_header_ustar ) + size + offset;
   }
   //while(1);
   //get a pointer to the beginning of the file
   if( isPresent == 1){
      //get a file descriptor from the list of available file
      //descriptors
      //get the file size

      for( int i = 0 ; i <= 99; i++ ){
             if( ! fdAllocArray[i] ){
                  fdAllocArray[i] = 1;
                  fdArr[ i ].fd = i;
                  fdArr[ i ].start= start;
                  fdArr[ i ].curPtr= start;
                  fdArr[ i ].fileSize = size;
                  fdArr[ i ].eof = 0;
                  *retVal = i;
                  return;
             }
      }
  
     *retVal = -1;
   }
   *retVal = -1;
}


void sys_read( struct regsForSyscall *regs ){
     

      int i = 0;
      int fd = (int)regs->rcx; 
      int nBytes = (int)regs->rdx;
      int *retNumBytesRead = (int *)regs->r10; 
      if( fd == -1  || !fdAllocArray[fd] ){
         printf("\nFD ALLOC ARRAY UNSET"); 
         //while(1); 
        *retNumBytesRead = 0; 
         return; 
      } 

      for( i=0; i <10; i++)
      {
             if( fdArr[i].fd == fd )
             {
                  if( fdArr[i].eof ){
                      *retNumBytesRead = 0;
                      return;
                  } 
                  else{
                      break;
                  }
             }
      }
      int numBytesRead = 0;
      char *buffPtr = (char *)regs->rax;
      char *temp = (char *)fdArr[i].curPtr;

      while( numBytesRead < nBytes && (( *temp) != '\0') ){
          *buffPtr = *temp;
          temp++;
          buffPtr++;
          *buffPtr = '\0';
          numBytesRead++;
          fdArr[i].curPtr = (uint64_t)temp;
          if( *temp == '\0' ){
             fdArr[ i ].eof = 1;
             //printf("\nEnd of file reached");
          }
      }
      *retNumBytesRead = numBytesRead; 


}

void sys_close( struct regsForSyscall *regs )
{
      int fd = (int)regs->rax; 
      int *retVal = (int *)regs->rcx; 
      fdAllocArray[ fd ] = 0;  
      *retVal = 1; 
}


void sys_opendir( struct regsForSyscall *regs){
    
    int *x  = (int *)regs->rcx; 
    char *dirname = (char *)regs->rax; 
    uint64_t start = (uint64_t)&_binary_tarfs_start;
    uint64_t end = (uint64_t)&_binary_tarfs_end;
    struct posix_header_ustar *ptr;
    int  isPresent = 0, size;
    ptr = (struct posix_header_ustar *)start;
    while( start < end  ){
       ptr = (struct posix_header_ustar *)start;
       if( !strcmp( ptr->name,dirname)){
            printf("\nEureka"); 
            start = start + sizeof(struct posix_header_ustar );
            size = 0; int i = 0;
            while( ptr->size[i] != '\0'){
                size = size*10 + ( ptr->size[i] - '0');
                i++ ;
            }
            if( size != 0 )
            {
                *x = -1;
            }
            size = octal_decimal( size ); 
            isPresent = 1;
            break;
       }
       size = 0; int i =0;
       while( ptr->size[i] != '\0'){
           size = size*10 + ( ptr->size[i] - '0');
           i++ ;
       }
       size = octal_decimal( size );
       int offset = 0;
       if( size % 512 != 0 )
       {
           offset =   512 - size % 512;
       }

       start = start + sizeof(struct posix_header_ustar ) + size + offset; 

    }

    if( isPresent == 1){
      //get a file descriptor from the list of available file
      //descriptors
      //get the file size

      for( int i = 1 ; i <= 99; i++ ){
             if( !dirDesc[ i ] ){
                  dirDesc[i] = 1;
                  dirIntArr[i].fd = i;
                  dirIntArr[i].startPos = start;
                  dirIntArr[i].curPos = start;        
                  dirIntArr[i].eofDirectory = 0;
                  dirIntArr[i].prevFile[0] = '\0';
                  strcpy( dirname , dirIntArr[i].dirPath );
                  printf("\n Ding Dong");  
                  //*dir =  &dirIntArr[i];
                  //printf("Hello ptr %p",*dir); 
                  *x = i; 
                  return; 
              }
      }
      *x =  -1;
   }
   *x =  -1;
}

void sys_readdir( struct regsForSyscall *regs ){
  
    DIR* dir = (DIR *)regs->rax;     
    char* buff = (char *)regs->rcx;
    if( dirDesc[dir->fd] == 0 )
    {
         *buff = '\0';
         return;
    } 
    getFileName( dir, buff); 
}

void sys_closedir( struct regsForSyscall *regs )
{
     int fd = (int)regs->rax; 
     int *retval = (int *)regs->rcx; 
     dirDesc[ fd ] = 0;
     *retval = 1;
}

static void *syscalls[NUM_SYSCALLS] =
{
     print,
     exitSyscall,
     sscanfSystemCall,
     enterToMemory,
     sys_fork,
     sys_execvpe,
     sys_sleep,
     sys_waitpid,
     sys_sbrk,
     sys_getpid,
     sys_open,
     sys_read,
     sys_close,
     sys_opendir,
     sys_readdir,
     sys_closedir,
};


void syscall_handler( struct regsForSyscall * s)
{
       //if (DEBUG) printf("SysCall handler begins\n");
       //if (DEBUG) printf("\nSyscall no %x",s->rbx);
       if( s->rbx > NUM_SYSCALLS )
         return;
       //if (DEBUG) printf("Inside Syscall handler\n");
       void (*location)(struct regsForSyscall*) =
           (void (*)(struct regsForSyscall*))syscalls[ s->rbx ];
       //if (DEBUG) printf("\nInside fault handler Alok\n");
       //if (DEBUG) printf("\nrax : %p",s->rax);
       //if (DEBUG) printf("\nrbx %p",s->rbx);
       //if (DEBUG) printf("\nrcx : %x",s->rcx);
       //if (DEBUG) printf("\nrdx %p",s->rdx); 
       location(s);
}


