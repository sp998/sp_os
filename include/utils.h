#ifndef UTILS_H
#define UTILS_H
#include<stddef.h>

// Custom itoa function
void itoa(int num, char *str);

// Copy a string
char* strcpy(const char* src,  char* dest );

int abs(int val);
void *memset(void *ptr, int value, size_t num);
#endif