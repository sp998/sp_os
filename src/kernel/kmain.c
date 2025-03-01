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

void myhandler(struct InterruptRegisters* regs){
    print("syscall\n");
    update_display();
}
void myhandler1(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler2(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler3(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler4(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler5(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler6(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler7(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler8(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler9(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler10(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler11(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler12(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler13(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler14(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}void myhandler15(struct InterruptRegisters* regs){
    print("syscall");
    update_display();
}

void my_process(){


    update_display();
    while(1);
}



void user_main(){
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();
    make_sys_call();

   //start_process((uint32_t)my_process, (uint32_t)user_stack + USER_STACK_SIZE);
  while(1);
}


void on_screen_reset(){
 printc(get_shell_prompt(),GREEN);
}


void kmain(){
    // Initialize critical system components
    
    init_gdt();   // Set up Global Descriptor Table
    install_syscall_handler(2,myhandler);
    initIdt();

    printc("In Kernel mode\n",GREEN);
    update_display();
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
