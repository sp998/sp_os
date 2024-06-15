#include "gdt.h"
#include "io.h"
#include "utils.h"
#include <stdarg.h>
#include "ide.h"


void display_dummy_text(int count){
    for(int i=0;i<count;i++){
        print("Number is:");
        print_number(i);
        print("\n");
    }
}
void kmain(){
 
    init_gdt();
    set_text_color(WHITE);
    clear_screen();


    init_idt();
    init_pic();
    enable_keyboard_interrupt();
    enable_interrupts();
    
    //printc("$p>",GREEN);
    printc("$p> ",GREEN);



    // Write the data buffer to the disk
    // if (ide_write_disk(sector, buffer, sector_count)) {
    //     print("Data written to disk successfully.\n");
    // } else {
    //     print("Failed to write data to disk.\n");
    // }
   //disk_write_sector(0,(char*)"Hello There");

   

  


    
    
   

   
  

update_display();
}