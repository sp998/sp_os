#include "utils.h"
#include "mem.h"
#include <string.h>
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


uint32_t min(uint32_t a, uint32_t b){
    if(a<b){
        return a;
    }else{
        return b;
    }
}

uint32_t max(uint32_t a, uint32_t b){
    if(a<b){
        return b;
    }else{
        return a;
    }
}

// Function to split a string into tokens
int split_string(const char *str, char delimiter, char tokens[][20], int max_tokens) {
    int token_count = 0;
    int char_index = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == delimiter) {
            tokens[token_count][char_index] = '\0'; // Null-terminate the token
            token_count++;
            char_index = 0;

            if (token_count >= max_tokens) {
                break; // Avoid overflow of the tokens array
            }
        } else {
            tokens[token_count][char_index++] = str[i];
        }
    }

    // Add the last token if there was no trailing delimiter
    tokens[token_count][char_index] = '\0'; // Null-terminate the last token
    return token_count + 1; // Return the total number of tokens
}

void split_name_ext(char* src, char* name, char* ext) {
    char* current = src; // Pointer to iterate through the source string
    char* name_ptr = name; // Pointer to copy characters to the name buffer
    char* ext_ptr = ext; // Pointer to copy characters to the ext buffer
    int found_dot = 0; // Flag to indicate if we found the dot

    // Iterate through each character in the source string
    while (*current != '\0') {
        if (*current == '.') {
            found_dot = 1; // Found the dot, so next characters are in the extension
            current++; // Move past the dot
            continue;
        }

        // Copy characters to name or ext based on whether dot has been found
        if (found_dot) {
            *ext_ptr++ = *current++;
        } else {
            *name_ptr++ = *current++;
        }
    }

    // Ensure null termination of strings
    *name_ptr = '\0';
    if (found_dot) {
        *ext_ptr = '\0'; // Null-terminate the extension string
    } else {
        *ext = '\0'; // If no dot, set the extension string to empty
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


char* strcombine(const char* src1,const char* scr2){
    size_t len1 = strlen(src1);
    size_t len2 = strlen(scr2);
    char* result = (char*)malloc(len1+len2+1);
    strcpy(src1,result);
    strcpy(scr2,result+len1);
    return result;
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


void outPortB(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

char inPortB(uint16_t port)
{
    char rv;
    __asm__ volatile("inb %1, %0" : "=a"(rv) : "Nd"(port));
    return rv;
}
