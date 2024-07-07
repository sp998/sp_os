int strcmp(char* str1,char *str2){
    while (*str1 &&(*str1==*str2))
    {   str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}