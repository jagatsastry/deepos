# include <stdio.h>
# include <files.h>
# include <defs.h>
# include <idt.h>
# include <string.h>
# include <sys/tarfs.h>

void getFileName( DIR *dir , char *buff )
{
    struct posix_header_ustar *ptr = ( struct posix_header_ustar *)dir->curPos; 
    uint64_t start = dir->curPos;
    if( dir->prevFile[0] == '\0' )
    {
         dir->prevFile[0] = '.';
         dir->prevFile[1] = '\0';
         *buff =  '.';
         buff++;
         *buff = '\0';
         return;
    }
   //within this while loop include the provision for checking the
   //end of file system once you figure it out
   //ptr = ptr +  512;
   fetchName( ptr->name, strlen( dir->dirPath ), buff );
   //printf("\nFetched Name %s",buff);
   //while(1);
   //int t = 0;
   while( contains( ptr->name, dir->dirPath )  )
   {
         fetchName( ptr->name, strlen( dir->dirPath ), buff );
         if(  strcmp( buff , dir->prevFile ) ){
             strcpy( buff,  dir->prevFile );
             //add the size of the pointer and the
             int size = 0; int i =0;
             while( ptr->size[i] != '\0'){
                size = size*10 + ( ptr->size[i] - '0');
                i++ ;
             }
            //printf("\nFuck it");
            size = octal_decimal( size );
             //printf("Size is %d",size);
             int offset = 0;
             if( size % 512 != 0 )
             {
                  offset =   512 - size % 512;
             }
             start = start + 512 + size + offset;
             dir->curPos = (uint64_t)start;
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
         ptr =(struct posix_header_ustar *)dir->curPos;
    }
    //printf("\nEnd of directory reached");
    buff[0] = '\0';
    dir->eofDirectory = 1;
}




void fetchName( const char *str1, int lenAdd ,char *buff){
   str1 = str1 + lenAdd;
  //char *buffPtr = buff;
   while( (*str1 != '\0') &&  (*str1 != '/') )
   {
       *buff = *str1;
       str1++;
       buff++;
   }
  *buff = '\0';
  //printf("\nString returned from fetchname is %s",buffPtr);

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

