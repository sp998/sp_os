#ifndef SYSVARS_H
#define SYSVARS_H
#include <stdint.h>
#include <stdbool.h>

uint16_t get_back_space_offset();
void set_back_space_offset(uint16_t value);

char* get_shell_prompt();
void set_shell_prompt(const char* prompt);
void set_current_key(char key);
char get_current_key();

void set_user_input_mode(bool value);
void set_user_input_buffer(char* value);
char* get_user_input_buffer();

bool get_user_input_mode();

set_key_pressed(bool value);
bool get_key_pressed();


#endif