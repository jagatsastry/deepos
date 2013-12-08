#ifndef _STDIO_H
#define _STDIO_H

int printf(const char *format, ... );
int scanf(const char *format, ...);
char* itoa( unsigned long int num ); 
void putc( char c); 
void write( int fd, void *buff);

#endif
