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
#include <kernel/syscalls/print_syscall.h>
#include<printf.h>
#include <ide.h>
#include <elf_loader.h>
#include <fat32.h>
#include <syslib.h>
#include <gui/init.h>
#include <kernel/syscalls/exit_syscall.h>
#include <kernel/syscalls/input_syscall.h>




extern void make_sys_call();
extern  char read_key();


void user_main(){
    printc("$p>",GREEN);
    set_back_space_offset(4);
    

    // uint8_t elf_buffer = (uint8_t*)USER_ELF_LOAD_ADDR;

    // fat_read_file_content(15,4620,elf_buffer);
    // uint32_t program_entry= load_and_print_elf(elf_buffer);
    // if(program_entry==NULL){
    //     printf("could not start program invalid entry point");
    // }else{
    //     start_process(program_entry, (uint32_t)user_stack + USER_STACK_SIZE);
    // }
    
    
    update_display();
    while(1);


   

}




void on_screen_reset(){

 printc(get_shell_prompt(),GREEN);
}

void kmain(uint32_t magic,multiboot_info_t* bootInfo){
    // Initialize critical system components

    //printc("In Kernel mode\n",GREEN);
    //print("setting up GDT\n");
    init_gdt();   // Set up Global Descriptor Table
    //print("setting up IDT\n");
    initIdt();


    init_ide();

    update_display();
    //init_mem_disk();
    //print("Setting up malloc\n");
    init_malloc();
    //init_root();
    //init_system_events();
    //init_timer();
    //print("Setting up Keyboard\n");
    init_keyboard();
    //print("setting up mouse\n");
    init_mouse();
    //print("setting up system calls\n");
    init_syscall_start_process();
    init_syscall_read_key();
    init_syscall_print();
    fs_init();
    init_exit_syscall();
    init_input_syscall();


   


    



    // Finalize display setup
    //update_display()

    
     
   //print("switching to user mode.\n");

   
   
    switch_to_user_mode();
}
