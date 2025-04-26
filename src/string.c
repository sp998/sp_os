#include <string.h>

int strcmp(const char* str1,char *str2){
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

char tolower_custom(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A'); // Convert uppercase to lowercase
    }
    return c; // Otherwise return as-is
}


int strcmpi(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        char c1 = *s1;
        char c2 = *s2;

        // manual tolower (only for uppercase A-Z)
        if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
        if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';

        if (c1 != c2) {
            return (unsigned char)c1 - (unsigned char)c2;
        }

        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}