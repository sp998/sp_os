#include <string.h>

int strcmp(char* str1,char *str2){
    while (*str1 &&(*str1==*str2))
    {   str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

uint32_t strlen(char* str){
    uint32_t count=0;
    while ((*str++)!='\0')
    {
        count++;
    }
    return count;
    
}
