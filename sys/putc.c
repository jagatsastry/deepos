int x_cord = 0, y_cord = 0;

void scroll();

void putc( char c )
{
   unsigned short *testMPtr = (unsigned short *)0xB8000;
   unsigned short *scrnLocPtr ;
   int attrb = 0x0F;
   // if there is a backspace handle it by moving the x_cord back
   // by one position
   if( c == 0x08 )
   {
      if (x_cord > 0){
          x_cord--;
      } else {
          if (y_cord > 0) 
              y_cord--;
          x_cord = 79;
      }
      scrnLocPtr = testMPtr + ( y_cord * 80 ) + x_cord;

      *scrnLocPtr = 0x20 | (attrb << 8);    
   }
   else if( c == '\n')
   {
   /*If there is a new line set the x_cord to 0 and increment y_cord
   by one */
       x_cord = 0;
       y_cord++;
   }
   else if( c == '\t')
   {
        if (x_cord + 8 > 79)
                y_cord++;
        x_cord = (x_cord + 8) % 80;                                                    
   }    
   /*If there is a printable character compute the appropriate memory
   location and save the characters value in the VGA Memory buffer*/
   else if( c >= ' ')
   {
      scrnLocPtr = testMPtr + ( y_cord * 80 ) + x_cord;
      *scrnLocPtr = c | attrb << 8;
      x_cord++;
   } 
   
   scroll(); 
}


void scroll()
{
    unsigned short *testMPtr = (unsigned short *)0xB8000;
        int attrib = 0x0F;
    int i ;
        unsigned blank; 
    blank = 0x20 | (attrib << 8);    
        if( y_cord > 24)
    {
          for( i = 0; i < 24 * 80; i++)
          {
             *testMPtr = *(testMPtr + 80);     
                  testMPtr++; 
              }
          for( i = 0; i < 80; i++){
             *testMPtr = blank;
             testMPtr++; 
          } 

                 x_cord = 0;
          y_cord = 24;  
    }
}
