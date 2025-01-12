#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

#define MAX_ARGS 10
#define STRING_BUFFER_SIZE 20

char** get_args(char* input, size_t length,int* argc);
void clear_args();

#endif // ARGS_H
