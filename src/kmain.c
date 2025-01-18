#include <gdt.h>
#include <io.h>
#include <utils.h>
#include <ide.h>
#include <mem.h>
#include <memdisk.h>
#include <system_events.h>
#include <sysvars.h>
#include <file.h>




void on_screen_reset(){
 printc(get_shell_prompt(),GREEN);
}

void kmain(){
    // Initialize critical system components
    init_gdt();                   // Set up Global Descriptor Table
    init_idt();                   // Set up Interrupt Descriptor Table
    init_pic();                   // Initialize Programmable Interrupt Controller
    
    // Enable interrupts for keyboard and other devices
    enable_keyboard_interrupt();
    enable_interrupts();          // Enable CPU interrupt handling
    
    // Set up dynamic memory management
    init_malloc();
    
    // Initialize the event-driven system for modular behavior
    init_system_events();

    // Prepare the display
    set_text_color(WHITE);        // Set default text color
              

    // Set up memory-backed disk for file system
    init_mem_disk();
    init_root();
    
    

    // Configure shell
    set_shell_prompt("$p>");      // Set shell prompt
    printc(get_shell_prompt(), GREEN); // Display the shell prompt in green

    // Subscribe to screen reset event to redraw the prompt
    subscribe_to_event(get_event(SCREEN_RESET_EVENT), on_screen_reset);
    
    //dispacth system ready event 


    // Finalize display setup
    update_display();             // Refresh the display with initial content
}
