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
    print("Do you want to enter graphics mode? Press Enter to confirm.");

    update_display();
    char key,key2;
    
    key = read_key();
    print_char(key);
    update_display();

    setVgaMode(320,200,8);
    setBackground(0x50);
    for (uint16_t x = 0; x < 320; x++) {
        
        uint16_t y = (x * 199) / 319;  
        putPixelWithIndex(x, y, 0xE); 
    }

    for (uint16_t x = 0; x < 320; x++) {
       
        uint16_t y = 199-(x * 199) / 319;  
        putPixelWithIndex(x, y, 0xE);  
    }


    for (uint16_t x = 0; x <= WIDTH ; x++) {
       
        uint16_t y1 = (x * HEIGHT) / WIDTH;

       
        uint16_t y2 = HEIGHT - (x * HEIGHT) / WIDTH;

       
        uint16_t y = min(y1, y2);
        uint16_t y3 = max(y1,y2);


           for(uint8_t i=0;i<100;i++){
            putPixelWithIndex(x, y-i, 0x2); 
            putPixelWithIndex(x,y3+i,0x2);
           }
        
    }


    for (uint32_t y = 0; y <= HEIGHT; y++) {
       
        uint32_t x1 = (y * WIDTH) / HEIGHT;
        uint16_t x2 = WIDTH - (y * WIDTH) / HEIGHT;

        uint32_t x = min(x1, x2);
        uint32_t x3 = max(x1,x2);

    
        for (uint32_t i = 0; i <=30; i++) {
            if(x-i<WIDTH/2){
            putPixelWithIndex(x-i, y, 0x2);
            }
        
            if(x3+i>WIDTH/2 && x3+i<WIDTH){
            putPixelWithIndex(x3+i,y,0x2);
            }
        
        }
    }
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
    



    // Finalize display setup
    //update_display();
    init_malloc();
     
   

   
   



 
    
    
   
   
    switch_to_user_mode();
}
