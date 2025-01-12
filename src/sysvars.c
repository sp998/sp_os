#include <sysvars.h>
#include <utils.h>
#include <string.h>

uint16_t back_space_offset=5;
char* shell_prompt;

uint16_t get_back_space_offset(){
     return back_space_offset;
}

void set_back_space_offset(uint16_t value){
    back_space_offset = value;
}

 char* get_shell_prompt(){
    return shell_prompt;
 }

 void set_shell_prompt(const char* prompt){
    strcpy(prompt,shell_prompt);
    set_back_space_offset(strlen(prompt)+1);
 }