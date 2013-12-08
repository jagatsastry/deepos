#include<pic.h>
#include <task.h>
#include<defs.h>
#include<stdio.h>
#include<kb.h>
#include<idt.h>

extern void print_key(char c);
extern void putc_bold(char c);

void keyboard_handler(struct regs *r)
{
    static uint08_t SHIFT_PRESSED = 0;

    unsigned char scancode;
    scancode = port_inb(0x60);
    //int *keyPressedPtr = (int *)r->r10;
    //printf("Valus of KeyPressed is %p", keyPressedPtr); 
    //printf("Scancode is %d",scancode);
    if (scancode & 0x80)
    {
        if(scancode - 0x80 == SHIFT_SCANCODE)
            SHIFT_PRESSED = 0;
    }
    else
    {
        if(scancode == SHIFT_SCANCODE) 
            SHIFT_PRESSED = 1;
        else 
        {
            unsigned char pressedKey = kbdus[scancode];
            if( scancode == 28){
               enterPressed = 1;
               if (DEBUG) printf("\nEntered Pressed");
               putc_bold('\n');

               return;
            } 

            if (SHIFT_PRESSED == 1) {
		    if(scancode >= 2 && scancode <= 13)
			    pressedKey = shiftNums[scancode - 2];
		    else {
			    switch(pressedKey) {
                    case '[': pressedKey = '{' ; break;
                    case ']': pressedKey = '}' ; break;
                    case ';': pressedKey = ':' ; break;
                    case '\'': pressedKey = '"'; break;
                    case ',': pressedKey = '<'; break;
                    case '.': pressedKey = '>'; break;
                    case '/': pressedKey = '?'; break;
                    default:
                        pressedKey = pressedKey - 32;
                }
            }
	    }
            if( enterPressed == 0 ){
                if (pressedKey == 0x08) {
                  curBuffIndex = (curBuffIndex > 0?curBuffIndex-1:curBuffIndex);
                  buff[curBuffIndex] = '\0';
                }
                else {
                  buff[curBuffIndex ] = pressedKey; 
                  curBuffIndex++; 
                }
                putc_bold(pressedKey);
                //print_key(pressedKey); 
            }else{
                putc_bold( pressedKey ); 
                //print_key(pressedKey);
            } 
           }
    }
}

