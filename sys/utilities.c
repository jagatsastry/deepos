#include<string.h>
#include<stdio.h>
#include <addr.h>

char* lpad(char* str)
{
    static char temp[3];
    int i = 0;
    for(; i < 3; i++)
        temp[i] = 0;
    int len = strlen(str);
    if(len <= 1) {
        temp[0] = '0';
        if(len == 1)
            temp[1] = str[0];
        else
            temp[1] = '0';
        temp[2] = '\0';
        return temp;
    }
    return str;
}


void print_key(char key) 
{
    char* preamble = "Uptime: ";

    unsigned char *scrnLocPtr =  (unsigned char*)(vga_virt_addr + ( 23 * 80 * 2) - 2 * (22 + strlen(preamble)) - 2 * 7);

    int j = 0;
    scrnLocPtr[j++*2] = '<';
    scrnLocPtr[j*2-1] = '7';
    j++;
    scrnLocPtr[j++*2] = key;
    j++;
    scrnLocPtr[j++*2] = '>';
    scrnLocPtr[j*2 - 1] = '7';
}

void print_time(int millis)
{
    int tot_seconds = millis/18;
    int hourInt = tot_seconds/3600, minInt = (tot_seconds/60)%60, secInt = tot_seconds%60;

    char* preamble = "Uptime: ";
    unsigned char *scrnLocPtr =  (unsigned char*)(vga_virt_addr + ( 23 * 80 * 2) - 2 * (22 + strlen(preamble)));
    
    int i = 0, j = 0;

    for( i = 0; preamble[i]; i++, j++) 
        scrnLocPtr[j*2] = preamble[i];

    char* hour = lpad(itoa(hourInt));
    for( i = 0; hour[i]; i++, j++) 
        scrnLocPtr[j*2] = hour[i];
    scrnLocPtr[j*2] = ':';
    j++;

    char* min = lpad(itoa(minInt));
    for( i = 0; min[i]; i++, j++) 
        scrnLocPtr[j*2] = min[i];
    scrnLocPtr[j*2] = ':';
    j++;

    char* sec = lpad(itoa(secInt));
    for( i = 0; sec[i]; i++, j++) 
        scrnLocPtr[j*2] = sec[i];
}