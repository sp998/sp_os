#ifndef SYSLIB_H
#define SYSLIB_H
#include <stdint.h>
#define USER_STACK_SIZE 4096
extern void start_process(uint32_t eip, uint32_t esp);
char get_user_stack();
#endif