# include <stdio.h>
# include <stdarg.h>

void puts( const char *str, int *num, va_list argp ); 
char* convertToHex(unsigned long int number );

int printf(const char *str, ...){
     int num = 0;
     va_list argp;
     va_start( argp, str );
     puts( str , &num , argp); 
     va_end( argp );
     return num;
}


//This function is to print the entire string
void puts( const char *str, int *num, va_list argp )
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
                               putc('%');
                               break;

                             case 'd':
                                      numInt = va_arg( argp, int );
				      if( numInt < 0 ){
				      	  putc('-');
				          numInt = -numInt;	
			              }		 
                                      stringizedNum = itoa( numInt);
                                      while( *stringizedNum != '\0')
                                      {
                                           putc( *stringizedNum );
                                           stringizedNum++;
                                           *num = *num + 1; 
                                      }
                                      break ;
                             case 'c':
                                      putc( va_arg( argp , int )); 
                                      *num = *num + 1; 
                                      break; 
                             case 's':
                                      str1 = va_arg( argp, char *);
                                      while( *str1 != '\0' ){
                                          putc( *str1 );
                                          str1++;
                                          *num = *num + 1;
                                      } 
                                      break; 
                             case 'p':
                                     intNumber = va_arg( argp, unsigned long int);
                                     stringizedNum = convertToHex( intNumber );
                                     while( *stringizedNum != '\0')
                                     {
                                           putc( *stringizedNum );
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
                                     stringizedNum = convertToHex( intNumber );
				     while( *stringizedNum != '\0')
                                     {
                                           putc( *stringizedNum );
                                           stringizedNum++;
                                           *num = *num + 1;
                                     }
                                     //break;
                       }
                       str++;
                       break; 
             default :
                          putc( *str );
                          str++; 
                          *num = *num + 1;
          } 
     }
}

char* convertToHex(unsigned long int number )
{
	static char buffForHex[ 64 ]; char tempChar, charToInsert= '\0';
	int curStringLoc = 2, hexRemainder, loc = 2;
	buffForHex[ 0 ] = '0';
	buffForHex[ 1 ] = 'x';
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
		buffForHex[ curStringLoc ] = charToInsert;
		curStringLoc++;
	}while( number != 0 ); 
	buffForHex[ curStringLoc ] = '\0';
        curStringLoc--;
	//Now reverse the string to get the correct hex value
        while( loc < curStringLoc )
        {
		tempChar = buffForHex[ loc ];
		buffForHex[ loc ] = buffForHex[ curStringLoc ];
		buffForHex[ curStringLoc ] = tempChar;
                loc++;
		curStringLoc--; 
        }
	return buffForHex;
}
