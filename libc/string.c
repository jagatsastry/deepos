#include<string.h>
#include<defs.h>
#include <stdio.h>
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

int startsWith(char *str, char *substr) {
  int a = strlen(str);
  int b = strlen(substr);
  if (b > a)
    return 0;

  for (int i = 0; i < b; i++)
    if (str[i] != substr[i])
      return 0;
  return 1;
}
char* strcat(char *str1, char *str2) {
  int a = strlen(str1);
  int b = strlen(str2);
  char *ret = (char*)malloc(a + b + 1);
  int i = 0;
  for (int j = 0; j < a; i++, j++)
    ret[i] = str1[j];
  for (int j = 0; j < b; i++, j++)
    ret[i] = str2[j];
  ret[i] = '\0';
  return ret;
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


char * strtok(char * str, char *comp)
{
  static int pos;
  static char *s;
  int  start = pos;

  // Copying the string for further calls of strtok
  if(str != NULL){
    s = str;
                pos = 0;
                if (DEBUG) printf("\nNew command entered %s",str);
                start = pos;
                if( *str == '\r')
                   return NULL;
        }
  //i = 0;
  int j = 0;
  //While not end of string
  while(s[pos] != '\0')
  {
    j = 0;
    //Comparing of one of the delimiter matches the character in the string
    while(comp[j] != '\0')
    {
      //Pos point to the next location in the string that we have to read
      if(s[pos] == comp[j])
      {
        //Replace the delimter by \0 to break the string
        s[pos] = '\0';
        pos = pos+1;
        //Checking for the case where there is no relevant string before the delimeter.
        //start specifies the location from where we have to start reading the next character
        if(s[start] != '\0')
          return (&s[start]);
        else
        {
          // Move to the next string after the delimiter
          start = pos;
          // Decrementing as it will be incremented at the end of the while loop
          pos--;
          break;
        }
      }
      j++;
    }
    pos++;
  }//End of Outer while
  s[pos] = '\0';
  if(s[start] == '\0')
    return NULL;
  else
    return &s[start];
}

int isspace(char a) {
  return a == ' ' || a == '\t' || a == '\n';
}

char *trim(char *str) {
  while(isspace(*str)) str++;
  if(*str == 0)
    return str;

  char *end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  *(end+1) = 0;

  return str;
}
