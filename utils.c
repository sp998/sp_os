#include "utils.h"
#include "mem.h"
void itoa(int num, char *str){
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly, otherwise empty string is printed for num = 0
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    // If number is negative, append '-'
    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Copy a string
char* strcpy(const char* src,  char* dest ) {
    char* p = dest;
    while (*dest != '\0') {
        *p++ = *src++;
    }
    *p = '\0';
    return dest;
}

int  abs(int val){
    if(val>0){
        return val;
    }
    return -1*val;
}

