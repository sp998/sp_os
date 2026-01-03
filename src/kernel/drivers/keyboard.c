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



static Command* commandHead = NULL;

void init_keyboard()
{
   irq_install_handler(1, keyboard_handler);
   commandHead = init_commands();
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
uint32_t keyboard_handler(struct InterruptRegisters *regs)
{
    char rawcode = inPortB(0x60);
    char scancode = rawcode & 0x7F;
    bool pressed = (rawcode & 0x80) == 0;

    // Handle shift press/release
    if (scancode == 0x2A || scancode == 0x36) { // Left or Right Shift
        shift_pressed = pressed;
        return (uint32_t)regs;
    }

    if (scancode == 0x0E) { // 0x0E is Backspace
        backspace();  // You need to implement this
        update_display();
        return (uint32_t)regs;
    }

    // Fix operator precedence and logic: (scancode == 0x1c) && pressed
    if(scancode == 0x1c && pressed){

        if(get_user_input_mode()){
            set_user_input_mode(false);
            return (uint32_t)regs;
        }
        char input[80];
        get_line(input);
        int argc;
        char** args = get_args(input,80,&argc);
       
        Command* currentCommand = get_command(args[0],commandHead);
        if(currentCommand !=NULL){
            print("\n");
            currentCommand->commandHandler(argc,args);
        }else{
            
            // Try to find program in /sp/programs using Rust FS
            char path_buffer[128];
            // Manually construct path since we might not have sprintf
            int i = 0;
            const char* prefix = "sp/programs/";
            // Copy prefix
            while(prefix[i] != '\0') {
               path_buffer[i] = prefix[i];
               i++; 
            }
            // Copy input command
            int j = 0;
            while(input[j] != '\0') {
                path_buffer[i] = input[j];
                i++;
                j++;
            }
            path_buffer[i] = '\0';
            
            extern bool rust_fs_find_path(const char* path);
            extern uint32_t rust_fs_load_file(const char* path, uint8_t* buffer);
            
            if (rust_fs_find_path(path_buffer)) {
                 // Load file
                 uint8_t* elf_buffer = (uint8_t*)USER_ELF_LOAD_ADDR;
                 uint32_t size = rust_fs_load_file(path_buffer, elf_buffer);

                 // We don't have extensive printf for ints, assume size > 0 check is enough for flow
                 if (size > 0) {
                     uint32_t program_entry = load_and_print_elf(elf_buffer);
                     if(program_entry != 0) {
                        start_process(program_entry, (uint32_t)get_user_stack() + USER_STACK_SIZE);
                     } else {
                         print("Failed to start process: invalid entry point\n");
                     }
                 } else {
                     print("Failed to load file content\n");
                 }
            } else {
                 print("\nUnknown command: ");
                 print(input); 
                 print("\n");
            }
        }
        
    
        
        printc("$p>",GREEN);
        scroll_up();
        scroll_up();
        show_buffer();
   
        return (uint32_t)regs;
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
    return (uint32_t)regs;
}