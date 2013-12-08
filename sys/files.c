# include <stdio.h>
# include <defs.h>
# include <syscall.h>
# include <files.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>

extern DIR  dirIntArr[100]; 

void getFileName( DIR *dir , char *buff )
{
    struct posix_header_ustar *ptr = ( struct posix_header_ustar *)dir->curPos; 
    uint64_t start = dir->curPos;
    int fd = dir->fd;
    if( !strcmp(dir->dirPath , "/"))
    {
        getRootSubdirectories( dir, buff); 
        return;
    } 
    if( dir->prevFile[0] == '\0' )
    {
         dirIntArr[fd].prevFile[0] =  dir->prevFile[0] = '.';
         dirIntArr[fd].prevFile[0] = dir->prevFile[1] = '\0'; 
         *buff =  '.';
         buff++;
         *buff = '\0';
         return; 
    } 
   //within this while loop include the provision for checking the
   //end of file system once you figure it out
   //fetchName( ptr->name, strlen( &(dir->dirPath[1]) ), buff );
   while( contains( ptr->name, &(dir->dirPath[1]) )  )
   {
         fetchName( ptr->name, strlen( &(dir->dirPath[1]) ), buff ); 
         if(  strcmp( buff , dir->prevFile ) ){ 
             strcpy( dir->prevFile, buff );
             strcpy( dirIntArr[fd].prevFile, buff); 
             //add the size of the pointer and the
             //printf("\nHulloa %s", dirIntArr[fd].prevFile); 
             int size = 0; int i =0;
             while( ptr->size[i] != '\0'){
                size = size*10 + ( ptr->size[i] - '0');
                i++ ;
             }
            size = octal_decimal( size );
             //printf("Size is %d",size);  
             int offset = 0;
             if( size % 512 != 0 )
             {
                offset =   512 - size % 512;
             } 
             start = start + 512 + size + offset;
             dir->curPos = (uint64_t)start;
             dirIntArr[fd].curPos =  (uint64_t)start; 
             //printf("\nPtr new",ptr->name); 
             //printf("The current file is %s",((struct posix_header_ustar *)dir->curPos)->name);  
             //printf("\nHello World %s",dir->prevFile);
             //while(1);
             return; 
         }
        // while(1);  
        //add the size of the pointer and the 
         int size = 0; int i =0;
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
         start = start + sizeof(struct posix_header_ustar ) + size +offset;
         dir->curPos = (uint64_t)start; 
         dirIntArr[fd].curPos = (uint64_t)start; 
         ptr =(struct posix_header_ustar *)dir->curPos; 
    }
    buff[0] = '\0';  
    dirIntArr[fd].eofDirectory = 1; 
    dir->eofDirectory = 1; 
}


void fetchName( const char *str1, int lenAdd ,char *buff){
   str1 = str1 + lenAdd;
   while( (*str1 != '\0') &&  (*str1 != '/') )
   {
       *buff = *str1;
       str1++;
       buff++;
   }
   *buff = '\0'; 
}


 

int octal_decimal( int num ){
     int rem = 0, i=0 , t = 0; uint64_t  pow = 1, decimal = 0;
     while( num != 0 ){
         rem = num % 10;
         num = num/10;
         pow = 1;
         for( i = 0; i < t ; i++)
         {
              pow = pow *8; 
         }
         t++;
         decimal = decimal + rem * pow; 
      }

     return decimal;
}

void getRootSubdirectories( DIR *dir, char *buff)
{
   uint64_t start = (uint64_t)dir->curPos;
   uint64_t end = (uint64_t)&_binary_tarfs_end;
   struct posix_header_ustar *ptr;
   int fd = dir->fd;
   ptr = (struct posix_header_ustar *)dir->curPos;
   while( start < end  ){
          fetchName( ptr->name, 0, buff );
          if(  strcmp( buff , dir->prevFile ) ){
             strcpy( dir->prevFile, buff );
             strcpy( dirIntArr[fd].prevFile, buff);
             int size = 0; int i =0;
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
            start = start + 512 + size + offset;
            dir->curPos = (uint64_t)start;
            dirIntArr[fd].curPos =  (uint64_t)start;
            return;
         }
         //add the size of the pointer and the
         int size = 0; int i =0;
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
         start = start + sizeof(struct posix_header_ustar ) + size +offset;
         dir->curPos = (uint64_t)start;
         dirIntArr[fd].curPos = (uint64_t)start;
         ptr =(struct posix_header_ustar *)dir->curPos;
 
   }
   buff[0] = '\0';
   dir->eofDirectory = 1;
   dirIntArr[fd].eofDirectory = 1;
}
