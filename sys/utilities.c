#include<string.h>
#include<stdio.h>
#include <task.h>

extern volatile uint64_t vga_virt_addr;

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

    unsigned char *scrnLocPtr =  (unsigned char*)(vga_virt_addr + ( 25 * 80 * 2) - 2 * (22 + strlen(preamble)) - 2 * 7);

    int j = 0;
    scrnLocPtr[j++*2] = '<';
    scrnLocPtr[j*2-1] = '7';
    j++;
    scrnLocPtr[j++*2] = key;
    j++;
    scrnLocPtr[j++*2] = '>';
    scrnLocPtr[j*2 - 1] = '7';
}
int attrib = 0x0F;
void setc_bold(unsigned short *dest, char src) {
  *dest = src | (attrib << 8);
}

void print_time(int millis)
{
    int tot_seconds = millis/18;
    int hourInt = tot_seconds/3600, minInt = (tot_seconds/60)%60, secInt = tot_seconds%60;

    char* preamble = "Uptime: ";
    unsigned short *scrnLocPtr =  (unsigned short*)(vga_virt_addr + ( 25 * 80 * 2) - 2 * (22 + strlen(preamble)));
    
    int i = 0, j = 0;

    for( i = 0; preamble[i]; i++, j++) 
        setc_bold(&scrnLocPtr[j], preamble[i]);

    char* hour = lpad(itoa(hourInt));
    for( i = 0; hour[i]; i++, j++) 
        setc_bold(&scrnLocPtr[j], hour[i]);
    setc_bold(&scrnLocPtr[j], ':');
    j++;

    char* min = lpad(itoa(minInt));
    for( i = 0; min[i]; i++, j++) 
        setc_bold(&scrnLocPtr[j], min[i]);
    setc_bold(&scrnLocPtr[j], ':');
    j++;

    char* sec = lpad(itoa(secInt));
    for( i = 0; sec[i]; i++, j++) 
        setc_bold(&scrnLocPtr[j], sec[i]);
}

void print_current_task()
{
    char* preamble = "PID: ";
    unsigned short *scrnLocPtr =  (unsigned short*)(vga_virt_addr + ( 24 * 80 * 2));
    
    int i = 0, j = 0;

    for( i = 0; preamble[i]; i++, j++) 
        setc_bold(&scrnLocPtr[j], preamble[i]);
    char *pid = itoa(current_task->id);
    i = 0;
    while(pid[i])
      setc_bold(&scrnLocPtr[j++], pid[i++]);

    char *totProcStr = " Tot proc: ";
    for( i = 0; totProcStr[i]; i++, j++) 
        setc_bold(&scrnLocPtr[j], totProcStr[i]);
    char *numTask = itoa(numtasks());
    i = 0;
    while(numTask[i]) setc_bold(&scrnLocPtr[j++], numTask[i++]);
}

void clear_line23() {
    unsigned short *scrnLocPtr =  (unsigned short*)(vga_virt_addr + ( 23 * 80 * 2));
    int i = 0;
    for (; i < 25; i++)
      scrnLocPtr[i] = ' ';
}

uint64_t oct_to_dec(char* oct) {
  uint64_t ret = 0;
  uint64_t base = 1;
  int i =11;
  while (oct[i] <= '0' || oct[i] > '9')
    i--;
  for(;i >= 0 ;i--) {
    ret += base*(oct[i] - '0');
    base *= 8;
  }
  return ret;
}

