#include <io.h>
#include <utils.h>
#include <ide.h>
#include <mem.h>
#include <memdisk.h>
#include <system_events.h>
#include <sysvars.h>
#include <file.h>



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
    
    
    // print("          ,-.----.               ,----..                \n");
    // print("  .--.--.   \\    /  \\             /   /   \\    .--.--.    \n");
    // print(" /  /    '. |   :    \\           /   .     :  /  /    '.  \n");
    // print("|  :  /`. / |   |  .\\ :         .   /   ;.  \\|  :  /`. /  \n");
    // print(";  |  |--`  .   :  |: |        .   ;   /  ` ;;  |  |--`   \n");
    // print("|  :  ;_    |   |   \\ :        ;   |  ; \\ ; ||  :  ;_     \n");
    // print(" \\  \\    `. |   : .   /        |   :  | ; | ' \\  \\    `.  \n");
    // print("  `----.   \\;   | |`-'         .   |  ' ' ' :  `----.   \\ \n");
    // print("  __ \\  \\  ||   | ;            '   ;  \\; /  |  __ \\  \\  | \n");
    // print(" /  /`--'  /:   ' |             \\   \\  ',  /  /  /`--'  / \n");
    // print("'--'.     / :   : :              ;   :    /  '--'.     /  \n");
    // print("  `--'---'  |   | :               \\   \\ .'     `--'---'   \n");
    // print("            `---'.|                `---`                   \n");
    // print("              `---`                         \n");
    // // Configure shell

    // print("WARNING\n");
    // print("This operating system has full, unristricted control over your device.Runnig this OS may:");
    // print("1. Access to all memory and storage.\n");
    // print("2. Control over all hardware components(e.g. CPU, GPU, disk,peripherals).\n");
    // print("Runnig this OS without care may lead to unintended data loss or damage to your device.\n");
     set_shell_prompt("$p>");
    // print("AUTHOR:SANJAY PAUDEL\n");

  
    printc(get_shell_prompt(), GREEN); // Display the shell prompt in green

    // Subscribe to screen reset event to redraw the prompt
    subscribe_to_event(get_event(SCREEN_RESET_EVENT), on_screen_reset);
    
    //dispacth system ready event 

      main();
     

    // Finalize display setup
    update_display();             // Refresh the display with initial content
}
