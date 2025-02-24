#ifndef UTILS_H
#define UTILS_H
#include<stddef.h>
#include<stdint.h>

// Custom itoa function
void itoa(int num, char *str);

// Copy a string
char* strcpy(const char* src,  char* dest );

int abs(int val);
void *memset(void *ptr, int value, size_t num);
void *memcpy(void *dest, const void *src, size_t n);
char* strcombine(const char* src1,const char* scr2);
void split_name_ext(char* scr,char* name,char* ext);
// Function to split a string into tokens
int split_string(const char *str, char delimiter, char tokens[][20], int max_tokens);
void outPortB(uint16_t port, uint8_t value);
char inPortB(uint16_t port);
struct InterruptRegisters{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;

};
#endif