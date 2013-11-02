#include <stdio.h>
#include <pic.h>

void fault_handler()
{
    printf("Div by 0");
    for(;;);
}
    
