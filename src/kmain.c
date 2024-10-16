#include <gdt.h>
#include <io.h>
#include <utils.h>
#include <stdarg.h>
#include <ide.h>
#include <mem.h>
#include <ehci.h>


void display_dummy_text(int count){
    for(int i=0;i<count;i++){
        print("Number is:");
        print_number(i);
        print("\n");
    }
}
void cause_stack_overflow(int n) {
    //char large_text[10*4*64*1024];  // Allocate a large amount of stack space per call (4 KB)
    
    char* large_text =malloc(10*4*64*1024*sizeof(char));

    
    if(large_text!=NULL){
    strcpy("this is a large text",large_text);
       print(large_text);
    }else{
        print("null pointer");
    }
    print("\n");

free(large_text);

char* large_text2 =malloc(10*4*64*1024*sizeof(char));

    if(large_text2!=NULL){
    strcpy("This is another large text",large_text2);
       print(large_text2);
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
    // enable_keyboard_interrupt();
    // enable_interrupts();
    // init_malloc();
    //set_free_blocks(3,sizeof(char)*10*4*64*1024);
    
    
    //printc("$p>",GREEN);
    // printc("$p> ",GREEN);

    pci_scan_for_ehci();




    // Write the data buffer to the disk
    // if (ide_write_disk(sector, buffer, sector_count)) {
    //     print("Data written to disk successfully.\n");
    // } else {
    //     print("Failed to write data to disk.\n");
    // }
   //disk_write_sector(0,(char*)"Hello There");

   

  


    
    
   

   
  

update_display();
}