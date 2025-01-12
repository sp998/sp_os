#include <gdt.h>
#include <io.h>
#include <utils.h>
#include <stdarg.h>
#include <ide.h>
#include <mem.h>
#include <memdisk.h>
#include <file.h>
#include <system_events.h>
#include <sysvars.h>


void on_screen_reset(){
 printc(get_shell_prompt(),GREEN);



}


void kmain(){
 
     init_gdt();
     init_idt();
     init_pic();
     enable_keyboard_interrupt();
     enable_interrupts();
     init_malloc();
     init_system_events();
     set_text_color(WHITE);
     clear_screen();

   
    init_mem_disk();
    File file = {.name="/"};
    wirte_to_mem_disk(0,&file,sizeof(File));
    File *buffer;
    read_from_mem_disk(0,(char*)buffer,sizeof(File));
    print(buffer->name);
    print(buffer->ext);
    print("\n");

    set_shell_prompt("/home/sanjay$");

    printc(get_shell_prompt(),GREEN);
    subscribe_to_event(get_event(SCREEN_RESET_EVENT),on_screen_reset);
     


    // Write the data buffer to the disk
    // if (ide_write_disk(sector, buffer, sector_count)) {
    //     print("Data written to disk successfully.\n");
    // } else {
    //     print("Failed to write data to disk.\n");
    // }
   //disk_write_sector(0,(char*)"Hello There");

update_display();

}