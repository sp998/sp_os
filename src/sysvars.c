#include <sysvars.h>
#include <utils.h>
#include <string.h>

uint16_t back_space_offset=5;
static char* shell_prompt;
char currentKey=0;
bool keyPressed = false;

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

 void set_current_key(char key)
 {
   currentKey=key;
 }

 char get_current_key()
 {
     return currentKey;
 }

 set_key_pressed(bool value){
     keyPressed =value;
 }

 bool get_key_pressed(){
         return keyPressed;
     }