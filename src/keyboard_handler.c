#include<io.h>
#include<commands.h>
#include<utils.h>
#include<idt.h>
#define KEYBOARD_DATA_PORT 0x60
#define PIC1_COMMAND 0x20
#define PIC_EOI 0x20




void keyboard_isr_handler() {
     uint8_t keycode = inb(KEYBOARD_DATA_PORT);
      if (keycode & 0x80) {
        outb(PIC1_COMMAND, PIC_EOI);
        return;
    }
    if(keycode==0x0E){
        backspace();
        outb(PIC1_COMMAND, PIC_EOI);
        show_buffer();
        return;
    
    }
    if(keycode==0x49){
        scroll_up();
         show_buffer();
        
      
       
    }

    if(keycode==0x51){
        scroll_down();
         show_buffer();
        
    }

    if(keycode==0x1c){
    char input[80];
    get_line(input);
    int argc;
    char** args = get_args(input,80,&argc);
    Command* commandHead = init_commands();
   
    Command* currentCommand = get_command(args[0],commandHead);
    if(currentCommand !=NULL){
        print("\n");
        currentCommand->commandHandler(argc,args);

    }else{
    print("\nUnknown command:");
    print(input); 
    print("\n");
    }
    

    
    printc(get_shell_prompt(),GREEN);
    scroll_up();
    scroll_up();
    show_buffer();
    outb(PIC1_COMMAND, PIC_EOI); 
    return;
    }
    
   
     //display_keycode(keycode);
     display_char(keycode);
     show_buffer();
    outb(PIC1_COMMAND, PIC_EOI); // Send end-of-interrupt (EOI) signal
}


