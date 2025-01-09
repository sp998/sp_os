#include <gdt.h>
#include <io.h>
#include <utils.h>
#include <stdarg.h>
#include <ide.h>
#include <mem.h>

void kmain(){
 
    init_gdt();
    set_text_color(WHITE);
    clear_screen();


    init_idt();
    init_pic();
     enable_keyboard_interrupt();
     enable_interrupts();
     init_malloc();
    //set_free_blocks(3,sizeof(char)*10*4*64*1024);
    
    
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