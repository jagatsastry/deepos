#ifndef FILES_H
#define FILES_H
#include<defs.h>


struct fdRecord{
     int fd;
     uint64_t start;
     uint64_t curPtr;
     int fileSize;
     int eof;
};

//struct fdRecord fdArr[100];
int curLocation ;

typedef struct dirInt{
   int fd;
   uint64_t startPos;
   uint64_t curPos;
   int eofDirectory;
   char dirPath[100]; 
   char prevFile[ 100 ];
}DIR;

//DIR dirIntArr[ 100 ];
void getSheBang( char *fname , char *buff );  
int octal_decimal(int num );
void fetchName(const char *str1, int lenAdd, char *buff);
void getFileName(DIR *dir, char *buff);
void funcForLS( const char *str );
char returnedFileName[100]; 
int octal_decimal( int num ); 
int open( const char *fname ); 
int read( int fd, void *buff, int nBytes );
DIR* opendir( const char *str1);  
int close( int fd );
void getFileName( DIR * dir, char *buff );
void fetchName( const char *str1, int lenAdd ,char *buff ); 
char* readAddr( DIR * dir );
int closedir( DIR *dir );
void handleRootLS();
void getRootSubdirectories( DIR *dir, char *buff);
void getSheBang( char *fname , char *buff );

#endif 
