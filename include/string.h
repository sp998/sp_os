
#ifndef STRING_H
#define STRING_H
#ifdef __cplusplus
extern "C" {
#endif
#include<stddef.h>
#include<stdint.h>
int strcmp(const char* str1,char* str2);
uint32_t strlen(char* str);
char tolower(char c);
int strcmpi(const char* a, const char* b);

#ifdef __cplusplus
}
#endif
#endif