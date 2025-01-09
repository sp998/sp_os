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
    while (*src != '\0') {
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


void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned long *d = dest;
    const unsigned long *s = src;

    // Copy 8 bytes (64-bits) at a time (assuming long is 64-bits)
    while (n >= sizeof(unsigned long)) {
        *d++ = *s++;
        n -= sizeof(unsigned long);
    }

    // Copy remaining bytes (less than sizeof(unsigned long))
    unsigned char *d_byte = (unsigned char *)d;
    const unsigned char *s_byte = (const unsigned char *)s;
    while (n--) {
        *d_byte++ = *s_byte++;
    }

    return dest;
}
