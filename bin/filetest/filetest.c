#include <stdio.h>
#include <files.h>
#include <idt.h>

int main() {
  int fd = open("bin/folder1/test1.txt");
  printf("The returned file descriptor is %d", fd);
  char buff[100];
  printf("\nData Fetched is");
  while( read(fd, buff, 20 ))
  {
      printf("%s",buff);
  }
  printf("\nClosing descriptor value is %d",close(fd));
  printf("\nReading the value from a closed descriptor %d",read(fd,buff,20));
  DIR* var = opendir("bin/");
  printf("\n Value of descriptor is %d",var->fd); 
  char *s ;//= readAddr( var );
  while(( s = readAddr(var))!= 0) 
  {
       printf("\nFetched filename is %s", s);
  } 
  printf("\nI am the boss");
  printf("\nThe returned value on close is %d", closedir( var ));  
  printf("\nTrying to read value after closing the descriptor %d",s); 
  //getSheBang("/bin/folder1/alok.txt", buff );
  //printf("\nTesting for SheBang %s", buff );  
  return 0;

}
