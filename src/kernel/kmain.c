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
#include <kernel/syscalls/info_syscall.h>


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
extern int make_sys_call();
extern void start_process(uint32_t eip, uint32_t esp);
void my_process();

void second_process(){
    printc("In second user mode\n",GREEN);
    update_display();
    while(1);
}
void my_process(){
    while(1);
}



void user_main(){
    init_gdt();
   //start_process((uint32_t)my_process, (uint32_t)user_stack + USER_STACK_SIZE);
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
    init_syscall_info();

    // Finalize display setup
    //update_display();
    init_malloc();
    update_display();


   
    switch_to_user_mode();
}
