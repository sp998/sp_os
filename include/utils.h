#ifndef UTILS_H
#define UTILS_H
#include<stddef.h>

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
extern void switch_to_usermode();
#endif