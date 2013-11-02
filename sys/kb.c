#include<pic.h>
#include<defs.h>
#include<stdio.h>
#include<kb.h>
#include<idt.h>

extern void print_key(char c);

void keyboard_handler(struct regs *r)
{
    static uint08_t SHIFT_PRESSED = 0;

    unsigned char scancode;

    scancode = port_inb(0x60);


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
        
            print_key(pressedKey);
        }
    }
}

