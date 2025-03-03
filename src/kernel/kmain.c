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
extern char read_key();
extern void start_process(uint32_t eip, uint32_t esp);
void my_process();



void myhandler(struct InterruptRegisters* regs){
    print("syscall\n");
   
    update_display();
}


void my_process(){
    print("Inside process\n");
    update_display();
    char key,key2;
    
    key = read_key();
    print_char(key);
    update_display();

    key2 = read_key();
    print_char(key2);
    update_display();

    update_display();
    while(1);
}



void user_main(){
    print("In user mode\n");
    update_display();
    start_process((uint32_t)my_process, (uint32_t)user_stack + USER_STACK_SIZE);
    while(1);
}




void on_screen_reset(){

 printc(get_shell_prompt(),GREEN);
}

void kmain(uint32_t magic,multiboot_info_t* bootInfo){
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
    init_syscall_start_process();
    init_syscall_read_key();
    
    print_number(bootInfo->vbe_mode);
    print("\n");
    print_hex(bootInfo->framebuffer_addr_low);
    print("\n");
    update_display();



    // Finalize display setup
    //update_display();
    init_malloc();
     
    setVgaMode(320,200,8);

    for(uint32_t y=0;y<200;y++)
        for(uint32_t x=0;x<320;x++)
            putPixel(x,y,0,0,0xA8);

   
    //switch_to_user_mode();
}
