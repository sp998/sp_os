#include <io.h>
#include <utils.h>
#include <ide.h>
#include <mem.h>
#include <memdisk.h>
#include <system_events.h>
#include <sysvars.h>
#include <file.h>
#include <idt.h>
#include <timer.h>
#include <gdt.h>
#include <kernel/drivers/keyboard.h>


#ifdef __cplusplus
extern "C" {
#endif

void main();  // Function prototype
void set_up_gtd();

#ifdef __cplusplus
}
#endif

#define USER_STACK_SIZE 1024
char user_stack[USER_STACK_SIZE];
extern void make_sys_call();
extern void start_process(uint32_t eip, uint32_t esp);
void my_process();

void second_process(){
    printc("In second user mode\n",GREEN);
    update_display();
    make_sys_call();

    while(1);
}
void my_process(){
    char* test = malloc(sizeof(char)*200);
    if(test==NULL){
        print("Empty");
        update_display();
        return;
    }
    test[0] = 'h';
    test[1] = 'i';
    test[2]  = '\0';

    print(test);
    update_display();
    while(1);
}



void user_main(){
   start_process((uint32_t)my_process, (uint32_t)user_stack + USER_STACK_SIZE);
  while(1);
}


void on_screen_reset(){
 printc(get_shell_prompt(),GREEN);
}

void kmain(){
    // Initialize critical system components
    
    init_gdt();   // Set up Global Descriptor Table
    initIdt();
    printc("In Kernel mode\n",GREEN);
    //init_mem_disk();
    init_malloc();
    //init_root();
    //init_system_events();
    //init_timer();
    init_keyboard();

    // Finalize display setup
    //update_display();
    init_malloc();

   
    switch_to_user_mode();
}
