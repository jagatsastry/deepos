#include<string.h>
#include<defs.h>

uint64_t strlen(const char *str)
{
    int len = 0;
    for(len = 0; str[len]; len++);
    return len;
}
