#include<string.h>
#include<defs.h>
#include <stdlib.h>

size_t strlen(const char *str)
{
    size_t len = 0;
    for(len = 0; str[len]; len++);
    return len;
}

void strcpy(char *dest, char *src) {
  int n = strlen(src);
  do {
    dest[n] = src[n];
  }while(n--);
}

int strcmp(char *s , char *t) { 
  int a = strlen(s);
  int b = strlen(t);
  if (a != b) return a - b;
  int i = 0;
  for (; i < a; i++) {
    if (s[i] != t[i]) 
      return s[i] - t[i];
  }
  return 0;
}


int contains( const char *str1, const char *str2) {
    while( *str1 && *str2 ) {
      if( *str1 != *str2) {
           return 0;
      }
      str1++;
      str2++;
    }

   if( (! *str2 ) )
   {
       return 1;
   }else{
      return 0;
   }
}

