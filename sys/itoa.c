/**
 * Apache License, Version 2.0
 * Authors: Alok Katiyar, Jagat Pudipedddi
 *
 */

char* itoa( int num )
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
