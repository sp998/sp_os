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
#include <kernel/drivers/keyboard.h>


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
    init_gdt();   // Set up Global Descriptor Table
    initIdt();
    init_mem_disk();
    init_malloc();
    init_root();
    init_system_events();
    init_timer();
    init_keyboard();
    printc("$p>",GREEN);

    // Finalize display setup
    update_display();            


}
