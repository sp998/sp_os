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


#ifdef __cplusplus
extern "C" {
#endif

void main();  // Function prototype
void set_up_gtd();

#ifdef __cplusplus
}
#endif


void on_screen_reset(){
 printc(get_shell_prompt(),GREEN);
}

void kmain(){
    // Initialize critical system components
    init_gdt();                   // Set up Global Descriptor Table
    //set_up_gtd();

    //init_idt();                   // Set up Interrupt Descriptor Table
    //init_pic();                   // Initialize Programmable Interrupt Controller
    initIdt();
    init_mem_disk();
    init_malloc();
    init_root();
    init_system_events();
    init_timer();

    // Finalize display setup
    update_display();             // Refresh the display with initial content


}
