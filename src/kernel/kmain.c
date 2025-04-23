#include <utils.h>
#include <ide.h>
#include <io.h>
#include <mem.h>
#include <memdisk.h>
#include <system_events.h>
#include <sysvars.h>
#include <file.h>
#include <idt.h>
#include <timer.h>
#include <gdt.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/syscalls/start_process.h>
#include <kernel/syscalls/read_key.h>
#include <multiboot.h>
#include <kernel/drivers/vga.h>
#include <kernel/drivers/mouse.h>


#ifdef __cplusplus
extern "C" {
#endif

void main();  // Function prototype

#ifdef __cplusplus
}
#endif

#define USER_STACK_SIZE 4096
char user_stack[USER_STACK_SIZE];
extern void make_sys_call();
extern  char read_key();
extern void start_process(uint32_t eip, uint32_t esp);
void my_process();



void myhandler(struct InterruptRegisters* regs){
    print("syscall\n");
   
    update_display();
}


void my_process(){
    print("Do you want to enter graphics mode? Press Enter to confirm.");
    update_display();
    read_key();
    main();
    while(1);
}



void user_main(){

    printc("In user mode\n",BLUE);
    update_display();

    start_process((uint32_t)my_process, (uint32_t)user_stack + USER_STACK_SIZE);
    while(1);
}




void on_screen_reset(){

 printc(get_shell_prompt(),GREEN);
}

void kmain(uint32_t magic,multiboot_info_t* bootInfo){
    // Initialize critical system components

    printc("In Kernel mode\n",GREEN);
    print("setting up GDT\n");
    init_gdt();   // Set up Global Descriptor Table
    install_syscall_handler(2,myhandler);
    print("setting up IDT\n");
    initIdt();

  
    update_display();
    //init_mem_disk();
    print("Setting up malloc\n");
    init_malloc();
    //init_root();
    //init_system_events();
    //init_timer();
    print("Setting up Keyboard\n");
    init_keyboard();
    print("setting up mouse\n");
    init_mouse();
    print("setting up system calls\n");
    init_syscall_start_process();
    init_syscall_read_key();
    



    // Finalize display setup
    //update_display()
     
   print("switching to user mode.\n");

   
   
    switch_to_user_mode();
}
