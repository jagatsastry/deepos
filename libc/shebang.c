# include <stdio.h>
# include <defs.h>
# include <syscall.h>
# include <files.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>


void getSheBang(char *fname , char *buff )
{
    int fd = open( fname );
    char c[10];
    read(fd, c, 1); 
    while(   c[0] == ' '|| c[0] == '\n' || c[0] == '\0' )
    {
         read( fd, c, 1); 
    }
    if( c[0] == '#'){
         read(fd,  c, 1 );
         if( c[0] == '!' ){
              read( fd, c, 1 );
              if( c[0] == ' ')
              {
                 close(fd);
                 return;
              } 
              while( c[0] != '\n' )
              {
                    *buff = c[0];
                     c[0] = '\0'; 
                     read( fd,c, 1);
                     buff++;
                     if( c[0] == '\0')
                     {
                         *buff = '\0' ;
                          close(fd);
                          return; 
                     } 
              }
              *buff = '\0';
         }else{
             close( fd );  
             return;
         } 
    } 
    close(fd); 
    return ; 
}   




