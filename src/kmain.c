#include <gdt.h>
#include <io.h>
#include <utils.h>
#include <stdarg.h>
#include <ide.h>
#include <mem.h>


void display_dummy_text(int count){
    for(int i=0;i<count;i++){
        print("Number is:");
        print_number(i);
        print("\n");
    }
}
void cause_stack_overflow(int n) {
    //char large_array[4*64*1024];  // Allocate a large amount of stack space per call (4 KB)
    
    char* large_text =malloc(4*64*1024*sizeof(char));

    if(large_text!=NULL){
    strcpy("this is a large text",large_text);
       print(large_text);
    }else{
        print("null pointer");
    }

 


    if(n!=0){
    // Recursively call itself
    cause_stack_overflow(n-1);
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
    init_malloc();
    
    
    //printc("$p>",GREEN);
    printc("$p> ",GREEN);
   cause_stack_overflow(0);



    // Write the data buffer to the disk
    // if (ide_write_disk(sector, buffer, sector_count)) {
    //     print("Data written to disk successfully.\n");
    // } else {
    //     print("Failed to write data to disk.\n");
    // }
   //disk_write_sector(0,(char*)"Hello There");

   

  


    
    
   

   
  

update_display();
}