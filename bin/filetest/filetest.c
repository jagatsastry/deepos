#include <stdio.h>
#include <files.h>
#include <idt.h>
# include<task.h>

int main() {
  int fd = open("/bin/folder1/test12.txt");
  printf("The returned file descriptor is %d", fd);
  char buff[100];
  while( read(fd, buff, 20 ))
  {
      printf("%s",buff);
  }
  printf("\nClosing descriptor value is %d",close(fd));
  printf("\nReading the value from a closed descriptor %d",read(fd,buff,20));
  DIR* var = opendir("/bin/");
  printf("\n Value of descriptor is %d",var->fd); 
  char *s ;
  //var->fd = 6;
  while(( s = readAddr(var))!= 0) 
  {
       printf("\nFetched directory name is %s", s);
  } 
  printf("\nI am the boss");
  printf("\nThe returned value on close is %d", closedir( var ));  
  readAddr(var); 
  //printf("\nTrying to read value after closing the descriptor %d",s); 
  //char buffPtr[20] = {0}; 
  //getSheBang("bin/folder1/test1.txt", buffPtr );
  //printf("\nTesting for SheBang %s", buffPtr );  
  //close(STDOUT_FD);
  //write(STDOUT_FD, "My name is alok"); 
  return 0;

}
