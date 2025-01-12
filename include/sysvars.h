#ifndef SYSVARS_H
#define SYSVARS_H
#include <stdint.h>

uint16_t get_back_space_offset();
void set_back_space_offset(uint16_t value);

char* get_shell_prompt();
void set_shell_prompt(const char* prompt);

#endif