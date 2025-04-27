#include <kernel/drivers/keyboard.h>
#include <idt.h>
#include <stdbool.h>
#include <sysvars.h>
#include <printf.h>
#include <io.h>
#include <fat32.h>
#include <syslib.h>

#include<commands.h>
#include <elf_loader.h>
#define PIC1_COMMAND 0x20
#define PIC_EOI 0x20
bool shift_pressed = false;

// Simple scancode -> character mapping
const char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // 0x00
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    // 0x0F
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',          // 0x1D
    0,  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,             // 0x2B
    '*', 0,  ' ', 0,   0,    0,   0,   0,   0,   0,   0,   0,   0,             // 0x39
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                      // 0x44
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                            // ...
};

// Same but when Shift is pressed
const char keyboard_map_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', // 0x00
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   // 0x0F
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',          // 0x1D
    0,  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,             // 0x2B
    '*', 0,  ' ', 0,   0,    0,   0,   0,   0,   0,   0,   0,   0,            // 0x39
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                     // 0x44
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};



void init_keyboard()
{
   irq_install_handler(1, keyboard_handler);

}

void handle_file(FAT16_DirEntry* file){
     uint8_t elf_buffer = (uint8_t*)USER_ELF_LOAD_ADDR;

    fat_read_file_content(file->first_cluster_lo,file->file_size,elf_buffer);
    uint32_t program_entry= load_and_print_elf(elf_buffer);
    if(program_entry==NULL){
        printf("could not start program invalid entry point");
    }else{
        print("\n");
        start_process(program_entry, (uint32_t)get_user_stack() + USER_STACK_SIZE);
    }
}
void keyboard_handler(struct InterruptRegisters *regs)
{
    char rawcode = inPortB(0x60);
    char scancode = rawcode & 0x7F;
    bool pressed = (rawcode & 0x80) == 0;

    // Handle shift press/release
    if (scancode == 0x2A || scancode == 0x36) { // Left or Right Shift
        shift_pressed = pressed;
        return;
    }

    if (scancode == 0x0E) { // 0x0E is Backspace
        backspace();  // You need to implement this
        update_display();
        return;
    }

    if(scancode==0x1c&pressed){

        if(get_user_input_mode()){
            set_user_input_mode(false);
            outb(PIC1_COMMAND, PIC_EOI); 
            return;
        }
        char input[80];
        get_line(input);
        int argc;
        char** args = get_args(input,80,&argc);
        Command* commandHead = init_commands();
       
        Command* currentCommand = get_command(args[0],commandHead);
        if(currentCommand !=NULL){
            print("\n");
            outb(PIC1_COMMAND, PIC_EOI); 
            currentCommand->commandHandler(argc,args);
        }else{
         FAT16_DirEntry* file_result=fat_find_file(input);
         if(file_result==NULL){
        print("\nUnknown command:");
        print(input); 
      
         }else{
            outb(PIC1_COMMAND, PIC_EOI); 
            handle_file(file_result);
         }
         print("\n");
        }
        
    
        
        printc("$p>",GREEN);
        scroll_up();
        scroll_up();
        show_buffer();
   
        return;
        }
        
       
         //display_keycode(keycode);

    if (pressed) {
        char c = shift_pressed ? keyboard_map_shift[scancode] : keyboard_map[scancode];
        if (c) {
            if(get_user_input_mode()){
               _put_buffer(c,get_user_input_buffer());
            }else{

            _print_char(c);
         
            }
            show_buffer();
           
        }
    }
}