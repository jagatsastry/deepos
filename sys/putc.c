#include<defs.h>
#include <stdio.h>

extern volatile uint64_t vga_virt_addr;

int x_cord = 0, y_cord = 0;

void scroll();
void update_cursor(int row, int col);

int in_kernel_mode() {
  uint64_t kernel = 0;
  __asm__ __volatile__("movq %%cs, %0" : "=r"(kernel));

  return kernel != 0x1B;
}

void putc_with_attr( char c , short attr);

void putc( char c ) {
  putc_with_attr(c, 0);
}

void putc_bold(char c) {
  putc_with_attr(c, 0x0800);
}

void putc_with_attr( char c , short attr)
{
   unsigned short *testMPtr = (unsigned short *)vga_virt_addr;
   unsigned short *scrnLocPtr ;
   int attrb = 0x00;
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
      *scrnLocPtr = c | attr;
      x_cord++;
   } 
   
   scroll(); 
   if (in_kernel_mode())
     update_cursor(y_cord, x_cord);
}
extern void port_outb(uint16_t port, uint08_t data) ;
 void update_cursor(int row, int col)
 {
    unsigned short position=(row*80) + col;
 
    // cursor LOW port to vga INDEX register
    port_outb(0x3D4, 0x0F);
    port_outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    port_outb(0x3D4, 0x0E);
    port_outb(0x3D5, (unsigned char )((position>>8)&0xFF));
 }

void scroll()
{
    unsigned short *testMPtr = (unsigned short *)vga_virt_addr;
        int attrib = 0x0F;
    int i ;
        unsigned blank; 
    blank = 0x20 | (attrib << 8);    
        if( y_cord > 22)
    {
          for( i = 0; i < 22 * 80; i++)
          {
             *testMPtr = *(testMPtr + 80);     
                  testMPtr++; 
              }
          for( i = 0; i < 80; i++){
             *testMPtr = blank;
             testMPtr++; 
          } 

                 x_cord = 0;
          y_cord = 22;  
    }
}
