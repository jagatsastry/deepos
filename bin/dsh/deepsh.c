#include <stdio.h>

#define NULL 0

char * strtok(char * str, char *comp);
void dsh(){
   int argc;
   char *argv[ 64 ]; 
   for( ; ; ){ 
       char* shellPrompt = "crazy OS $";
       printf("\n%s",shellPrompt);
       
       //accept the input from the user
       char command[ 256 ] = { 0 };
       int numBytes = scanf("%s", command );   
       //printk("%d",command); 
      if( command[0] == '\0'){ 
          printf("enterPressed");  
          continue;
       }
       if( numBytes < 0 ){
           printf("\nShell Error");
       }
       printf("\nCommand entered is %s",command);  
       argc = 0;
       //argv[ 64 ];
       argv[ argc ] = strtok( command, " \t\n"); 
       while( argc++ < 64 )
           if( !(argv[ argc ] = strtok( NULL, " \t\n" )) ) break;
       for(int i = 0; i < argc;i++){
           printf("\n%s", argv[i]);
       }
   //fork off a child process
    /*   int pid = fork();
       if( pid == 0 )
       {
           if( execv( argv[0], argv ) < 0){
               printf("Bad Command\n");
               exit(0);
           } 

       }      
   int status = 0;
   wait( &status ); 
   */ 
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
                printf("\nNew command entered %s",str); 
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

int main() {
  dsh();
  return 0;
}

