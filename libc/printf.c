# include <stdio.h>
# include <stdarg.h>

void convertStr( const char *str, int *num, char *targetString ,va_list argp );
char* convertToHex2(unsigned long int number );

int x = 10;
int printf(const char *format, ...) {
  //printf("xxxx %d\n", x);
     int  num = 0;
     char strToStore[1000];
     va_list argp;
     va_start( argp, format );
     convertStr( format , &num , strToStore ,argp);
     va_end( argp );
     __asm__( "movq $0x00, %%rbx;\
               movq %0, %%rdx;\
               movq $0x0C, %%rcx;\
               int $0x80;\
               "::"g" (strToStore):"rbx","rdx","rcx") ;
     return num;  
}

/**
 * Apache License, Version 2.0
 * Authors: Alok Katiyar, Jagat Pudipedddi
 *
 */

char* itoa(unsigned long int num)
{
      static char buf[33];
      int i = 0;
      for(i = 0; i < 33; i++)
          buf[i] = 0;
      int  position = 0;
      int  startPos = 0;
      char temp;
      do
      {
          buf[ position ] =  num % 10 + '0' ;
          num = num / 10 ;
          position++;
      }  while( num > 0 );
      buf[ position ] = '\0';
      //reverse the string
      position--;
      while( startPos < position )
      {
         temp = buf[ startPos ];
         buf[ startPos] = buf[ position ];
         buf[ position ] = temp;
         position--;
         startPos++;
      }
      return buf;
}
//This function is to print the entire string
void convertStr( const char *str, int *num, char *targetString , va_list argp )
{
     char *stringizedNum;
     unsigned long int intNumber;
     unsigned long int numInt;
     char *str1;
     while( *str != '\0')
     {
          switch( *str ){
             case '%' :
                       str++;
                       switch( *str )
                       {
                           case '%':
                               //putc('%');
                               break;

                             case 'd':
                                      numInt = va_arg( argp, int );
                                      if( numInt < 0 ){
                                          //putc('-');
                                          numInt = -numInt;
                                      }
                                      stringizedNum = itoa( numInt);
                                      while( *stringizedNum != '\0')
                                      {
                                           *targetString = *stringizedNum;
                                           targetString++;
                                           //putc( *stringizedNum );
                                           stringizedNum++;
                                           *num = *num + 1;
                                      }
                                      break ;
                            case 'c':
                                      //putc( va_arg( argp , int ));
                                      *targetString = va_arg( argp, int );
                                      targetString++;
                                      *num = *num + 1;
                                      break;
                             case 's':
                                      str1 = va_arg( argp, char *);
                                      while( *str1 != '\0' ){
                                          //putc( *str1 );
                                          *targetString = *str1;
                                          targetString++;
                                          str1++;
                                          *num = *num + 1;
                                      }
                                      break;
                             case 'p':
                                     intNumber = va_arg( argp, unsigned long int);
                                     stringizedNum = convertToHex2( intNumber );
                                     while( *stringizedNum != '\0')
                                     {
                                           //putc( *stringizedNum );
                                           *targetString = *stringizedNum;
                                           targetString++;
                                           stringizedNum++;
                                           *num = *num + 1;
                                     }
                                     break;

                             case 'x':
                                     numInt = va_arg( argp, long int );
                                     if( numInt < 0 )
                                     {
                                              intNumber = -numInt;
                                              intNumber =  intNumber ^ 0xFFFF;
                                              intNumber =  intNumber + 1;
                                      }else{
                                             intNumber = numInt;
                                     }
                                     stringizedNum = convertToHex2( intNumber );
                                     while( *stringizedNum != '\0')
                                     {
                                           //putc( *stringizedNum );
                                           *targetString = *stringizedNum;
                                           targetString++;
                                           stringizedNum++;
                                           *num = *num + 1;
                                     }
                                     //break;
                       }
                       str++;
                       break;
             default :
                          //putc( *str );
                          *targetString = *str;
                          targetString++;
                          str++;
                          *num = *num + 1;
          }
     }
     *targetString = '\0';
}


char* convertToHex2(unsigned long int number )
{
        static char buffForHex[ 64 ]; char tempChar, charToInsert= '\0';
        int curStringLoc = 2, hexRemainder, loc = 2;
        buffForHex[ 0 ] = '0';
        buffForHex[ 1 ] = 'x';
                //printf("\nx %d\n", curStringLoc);
        do{
                hexRemainder = number % 16;
                number = number /16;
                if( hexRemainder > 9 )
                {
                        if( hexRemainder == 10 )
                                charToInsert = 'A';
                        if( hexRemainder == 11 )
                                charToInsert = 'B';
                        if( hexRemainder == 12 )
                                charToInsert = 'C';
                        if( hexRemainder == 13 )
                                charToInsert = 'D';
                        if( hexRemainder == 14 )
                                charToInsert = 'E';
                        if( hexRemainder == 15 )
                                charToInsert = 'F';

                }else{
                        charToInsert = hexRemainder + '0';

                }
                //printf("\nx %d\n", curStringLoc);
                buffForHex[ curStringLoc ] = charToInsert;
                curStringLoc++;
        }while( number != 0 );
        buffForHex[ curStringLoc ] = '\0';
        curStringLoc--;
        //Now reverse the string to get the correct hex value
        while( loc < curStringLoc )
        {
          //printf("y %d %d\n", loc, curStringLoc);
                tempChar = buffForHex[ loc ];
                buffForHex[ loc ] = buffForHex[ curStringLoc ];
                buffForHex[ curStringLoc ] = tempChar;
                loc++;
                curStringLoc--;
        }
        return buffForHex;
}



