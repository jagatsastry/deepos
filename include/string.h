#ifndef STRING_H
#define STRING_H
#include <defs.h>

size_t strlen(const char *str);
int strcmp(char *s , char *t);
void strcpy(char *dest, char *src);
int contains( const char *str1, const char *str2);
char * strtok(char * str, char *comp);
char* strcat(char *str1, char *str2);
int startsWith(char *str, char *substr);
char *trim(char *str);

#endif
