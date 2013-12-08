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

void exitSyscall(struct regsForSyscall *regs) {
    kexit(*(uint64_t*)regs->rdx);
}

extern void kps(char *options);
void sys_ps(struct regsForSyscall *regs) {
    volatile char *options = (volatile char*)regs->rdx;
    kps((char*)options);
}

void sys_cd(struct regsForSyscall *regs) {
    volatile char *dir = (volatile char*)regs->rdx;
    /*
    char *newDir = (char*)current_task->pwd;
    if (strcmp((char*)current_task->pwd, "/"))
      newDir = strcat((char*)current_task->pwd, "/");
    newDir = strcat(newDir, (char*)dir);*/
    strcpy((char*)current_task->pwd, (char*)dir);
}

void sys_pwd(struct regsForSyscall *regs) {
  volatile char *cwd = (volatile char*)regs->rdx;
  strcpy((char*)cwd, (char*)current_task->pwd);
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
  pid_tt pid = (pid_tt)s->rdx;
  pid_tt *retPid = (uint32_t*)s->rsi;
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
  *(uint64_t*)s->rdx = (uint64_t)brk();
}

void sys_getpid(struct regsForSyscall * s) {
  *(uint64_t*)s->rdx = current_task->id;
}

void sys_execvpe(struct regsForSyscall * s) {
  char *filename = (char*)s->rdx;
  char **argv = (char**)s->rcx;
  char **argp = (char**)s->rsi;
  int *ret = (int*)s->rdi;
  if (DEBUG) printf("In syscall execvpe: Arguments:  %x %x\n",  argv, argp);
  if (DEBUG) printf("In syscall execvpe: Arguments:   %s\n",   argv[0]);

  if (DEBUG) printf("syscall: Running execvpe of %s\n", filename);
  int argc = 0;
  for(; argv[argc]; argc++);

  *ret = kexecvpe(filename, argc, argv, argp);
  if (DEBUG) printf("In syscall-execvpe. Going back\n");
}

void sys_ulimit(struct regsForSyscall *regs) {
  size_tt lim = (size_tt)regs->rdx * 1024; //Input is in numkb
  size_tt taskCode = (uint64_t)regs->rcx;

  if (DEBUG) printf("Setting ulimit task code: %d, lim %x\n", taskCode, lim);
  if (current_task->current_mem_usage > lim)
    printf("Err: Current memory usage is more than the limit being set.");
  else current_task->mem_limit = lim;
}

void sys_kmalloc(struct regsForSyscall *regs) {
  uint64_t size = (size_tt)regs->rdx;
  uint64_t *ptr = (uint64_t*)regs->rcx;
  *ptr = (uint64_t)kmalloc(size);
}



struct fdRecord fdArr[100];
DIR dirIntArr[ 100 ];

int fdAllocArray[100] = {0};
int curLocation = 0;
int dirDesc[ 100 ] = {0};


void sys_noop(struct regsForSyscall *r) {
  if (DEBUG) 
    printf("n00p %d\n", r->rbx);
}



void sys_open( struct regsForSyscall *regs )
{
   char *fname = (char *)regs->rax; 
   int *retVal = (int *)regs->rcx; 
   uint64_t start = (uint64_t)&_binary_tarfs_start;
   uint64_t end = (uint64_t)&_binary_tarfs_end;
   struct posix_header_ustar *ptr;
   int  isPresent = 0, size ;
   ptr = (struct posix_header_ustar *)start;
   printf("\nFname is %s",fname);
   while( start < end  ){
       ptr = (struct posix_header_ustar *)start;
       if( !strcmp( ptr->name,&fname[1])){
            printf("\nFile found"); 
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

      for( int i = 3 ; i <= 99; i++ ){
             if( ! fdAllocArray[i] ){
                  printf("\nDescriptor found %d", i); 
                  fdAllocArray[i] = 1;
                  fdArr[ i ].fd = i;
                  fdArr[ i ].start= start;
                  fdArr[ i ].curPtr= start;
                  fdArr[ i ].fileSize = size;
                  fdArr[ i ].eof = 0;
                  current_task->open_files[i] = 1;   
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
      if( fd == -1  || ! current_task->open_files[fd]){ 
         printf("\nInvalid file descriptor"); 
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
      printf("\nValue of fd was %d",current_task->open_files[fd]); 
      current_task->open_files[fd] = 0;
       
      fdAllocArray[ fd ] = 0;  
      *retVal = 1; 
}


void sys_opendir( struct regsForSyscall *regs){
    
    int *x  = (int *)regs->rcx; 
    char *dirname = (char *)regs->rax; 
    DIR *dir = (DIR *)regs->rdx; 
    uint64_t start = (uint64_t)&_binary_tarfs_start;
    uint64_t end = (uint64_t)&_binary_tarfs_end;
    struct posix_header_ustar *ptr;
    int  isPresent = 0, size;
    if( !strcmp(dirname,"/")) { 
       for( int i = 3 ; i <= 99; i++ ){
             if( !dirDesc[ i ] ){
                  dirDesc[i] = 1;
                  dir->fd = dirIntArr[i].fd = i;
                  dir->startPos = dirIntArr[i].startPos = (uint64_t)start;
                  dir->curPos = dirIntArr[i].curPos = (uint64_t)start;
                  dir->eofDirectory = dirIntArr[i].eofDirectory = 0;
                  dir->prevFile[0] = dirIntArr[i].prevFile[0] = '\0';
                  strcpy(  dirIntArr[i].dirPath, dirname );
                  strcpy(  dir->dirPath , dirname);
                  current_task->open_directories[i] = 1;
                   printf("\n Ding Dong");
                  //*dir =  &dirIntArr[i];
                  //printf("Hello ptr %p",*dir);
                  *x = i;
                  return;
              }
      }     
    } 
    //while(1); 
    ptr = (struct posix_header_ustar *)start;
    while( start < end  ){
       ptr = (struct posix_header_ustar *)start;
       if( !strcmp( ptr->name,&dirname[1])){
            if (DEBUG) printf("\nEureka"); 
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

      for( int i = 3 ; i <= 99; i++ ){
             if( !dirDesc[ i ] ){
                  dirDesc[i] = 1;
                  dir->fd = dirIntArr[i].fd = i;
                  dir->startPos = dirIntArr[i].startPos = (uint64_t)start;
                  dir->curPos = dirIntArr[i].curPos = (uint64_t)start;        
                  dir->eofDirectory = dirIntArr[i].eofDirectory = 0;
                  dir->prevFile[0] = dirIntArr[i].prevFile[0] = '\0';
                  strcpy(  dirIntArr[i].dirPath, dirname );
                  strcpy(  dir->dirPath , dirname); 
                  current_task->open_directories[i] = 1; 
                   if (DEBUG) printf("\n Ding Dong");  
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
    if( !current_task->open_directories[dir->fd]  )
    {
         printf("\nInvalid directory descriptor for out process"); 
         *buff = '\0';
         return;
    }  
    if( dirIntArr[dir->fd].eofDirectory )
    {      
           *buff = '\0'; 
           return;  
   }
   //printf("\nJust before get FIle name"); 
    getFileName( dir, buff); 
}

void sys_closedir( struct regsForSyscall *regs )
{
     int fd = (int)regs->rax; 
     int *retval = (int *)regs->rcx; 
     current_task->open_directories[fd]; 
     dirDesc[ fd ] = 0;
     *retval = 1;
}

void sys_write(struct regsForSyscall *regs ){
     int fd = (int)regs->rcx; 
     char *buff = (char *)regs->rdx; 
     printf("\nCurrent taks open fd %d ",current_task->open_files[fd]); 
     if( !current_task->open_files[fd] )
     {
         printf("\n The descriptor %d is closed for the process. Cannot Write to it", fd);  
         return;
     }else{
         while( *buff != '\0' )
         {
              putc( *buff );
              buff++;
         }
     } 
}

 
void sys_ls( struct regsForSyscall *regs )
{

    //int *x  = (int *)regs->rcx;
    char *dirname = (char *)regs->rax;
    //printf("\nDirname %s",dirname);
    //while(1); 
    //DIR *dir = (DIR *)regs->rdx;
    uint64_t start = (uint64_t)&_binary_tarfs_start;
    uint64_t end = (uint64_t)&_binary_tarfs_end;
    char prevFile[100]= {0};
    prevFile[0] = '.';
    //printf("\n%s",prevFile); 
    if( !strcmp(dirname,"/"))
    {
        handleRootLS(); 
        if (DEBUG) printf("\nHello World");
        return;
    } 

    char buff[ 100 ] = {0};
    struct posix_header_ustar *ptr;
    int   size;
    ptr = (struct posix_header_ustar *)start;
    //printf(" 
    while( start < end  ){
       ptr = (struct posix_header_ustar *)start;
       if( contains(  ptr->name, &dirname[1] )){
            //printf("\nEureka %s", ptr->name);
            //printf("\nStrlen %d", strlen(dirname)); 
            fetchName(ptr->name,strlen(&dirname[1]),buff) ;
            //printf("\nKattu %s",buff); 
            //printf("\nBuff %s \n preFile %s \n curName",buff, prevFile, ptr->name );
            //while(1);  
            if( strcmp(buff, prevFile) ){
                 printf("\n%s",buff); 
                 strcpy(prevFile, buff);
                 //printf("PrevFile %s",prevFile);
                 //while(1); 
            } 
            //while(1); 
            //start = start + sizeof(struct posix_header_ustar );
            //size = 0; int i = 0;
            /*while( ptr->size[i] != '\0'){
                size = size*10 + ( ptr->size[i] - '0');
                i++ ;
            }
            if( size != 0 )
            {
                *x = -1;
            }
            size = octal_decimal( size );
            isPresent = 1; */
            //break;
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

    //printf("\nHello World");

    //while(1);
}

void handleRootLS(){
     uint64_t start = (uint64_t)&_binary_tarfs_start;
     uint64_t end = (uint64_t)&_binary_tarfs_end;
     struct posix_header_ustar *ptr; 
     char prevFile[100] = {0};
     char buff[100] = {0}; 
     while( start < end-512  ){
          ptr = (struct posix_header_ustar *)start;
          fetchName(ptr->name,0,buff) ;
          if( strcmp(buff, prevFile) ){
                 printf("\n%s",buff);
                 strcpy(prevFile, buff);
          }
            //while(1);
          int size = 0,offset =0; int i = 0;
          while( ptr->size[i] != '\0'){
              size = size*10 + ( ptr->size[i] - '0');
              i++ ;
          }
          size = octal_decimal( size );
          if( size % 512 != 0 )
          {
              offset =   512 - size % 512;
          }
          start = start + sizeof(struct posix_header_ustar ) + size + offset; 
     }
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
     sys_ulimit,
     sys_kmalloc,
     sys_ls,
     sys_write,
     sys_noop,
     sys_noop,
     sys_ps,
     sys_cd,
     sys_pwd,
};


void syscall_handler( struct regsForSyscall * s)
{
       if( s->rbx > NUM_SYSCALLS ) {
         printf("ERR: Invalid syscall %d\n", s->rbx);
         return;
       }
       void (*location)(struct regsForSyscall*) =
           (void (*)(struct regsForSyscall*))syscalls[ s->rbx ];
       location(s);
}
