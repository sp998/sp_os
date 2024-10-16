#include <io.h>
#include <utils.h>
#include <string.h>
#include <args.h>
#include <filesystem.h>

#define COMMAND_PORT 0x3D4
#define DATA_PORT 0x3D5

#define HIGH_BYTE_COMMAND  14
#define LOW_BYTE_COMMAND 15

#define VIDEO_MEMORY 0x000B8000
#define IDT_SIZE 256

#define COLS 80
#define ROWS 25

#define BACKSPACE_OFFSET 5

#define KEYBOARD_DATA_PORT 0x60
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC_EOI 0x20
#define BUFFER_SIZE 80000
#define DISPLAY_OFFSET 38000
#define INPUT_BUFFER_SIZE COLS*2
#define  scroll_size 1

static int current_height=0;


static uint32_t scroll_pos=DISPLAY_OFFSET;
uint8_t text_color=WHITE;
uint8_t background_color=BLACK;

char vid_buffer[BUFFER_SIZE];
uint32_t buffer_input_index;
char buffer_input[INPUT_BUFFER_SIZE];

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idt_p;
static uint16_t last_pos = 0;
int clamp(int a,int b,int c);
void display_buff_c(const char* str,uint8_t color);


void full_scroll();
int cur_col();

void reset_shell();
void shift_buffer(int source, int destination, int depth);
// Function to set an entry in the IDT
void set_idt_entry(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment offset
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // Interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

// Function to initialize the IDT
void init_idt() {
    idt_p.limit = sizeof(struct idt_entry) * IDT_SIZE - 1;
    idt_p.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_SIZE; i++) {
        set_idt_entry(i, 0);
    }

    // Set the keyboard ISR to entry 33 (IRQ1 + 32 for IDT offset)
    set_idt_entry(33, (uint32_t)keyboard_isr);

    load_idt((uint32_t)&idt_p);
}


// Function to enable interrupts
void enable_interrupts() {
    __asm__("sti"); // Set the Interrupt Flag (IF) bit in the flags register
}
void reset_scroll(){
    shift_buffer(scroll_pos,DISPLAY_OFFSET,get_height());
    scroll_pos=DISPLAY_OFFSET;
    last_pos=0;

}

void reset_shell(){
    for(int i=0;i<BUFFER_SIZE;i+=2){
        vid_buffer[i]='\0';
    }
    last_pos=0;
    scroll_pos=0;
    current_height=0;
    buffer_input_index=0;
 
    printc("$p> ",GREEN);
    

}

void clear_screen(){
    char *vid_mem = vid_buffer;
    int i=0;
    while (i<COLS*2*get_height())
    {
     if(scroll_pos==0){
      vid_mem[i+DISPLAY_OFFSET]='\0';
     }else{
        vid_mem[scroll_pos+i]= '\0';
     }
      i+=2;

    }
    last_pos=0;
    move_cursor(last_pos/2);
    
}


void backspace(){

       if(cur_col()<BACKSPACE_OFFSET){
        return;
       }
        buffer_input[buffer_input_index-1] = '\0';
        if(buffer_input_index>0){
            buffer_input_index--;
            
        }    
    
    write_to_cell(last_pos-2,'\0');
    if(last_pos!=0){
    last_pos-=2;
    }
    int cursor_pos=last_pos/2;
      int row = (cursor_pos/COLS);
      row =clamp(row,(ROWS/2+12),(ROWS/2+12));
      int col = (cursor_pos%COLS);

      move_cursor(row*COLS+col);

    
}

void move_cursor(unsigned short pos){
    outb(COMMAND_PORT,HIGH_BYTE_COMMAND);
    outb(DATA_PORT,((pos>>8)&0x00FF));
    outb(COMMAND_PORT,LOW_BYTE_COMMAND);
    outb(DATA_PORT,pos&0x00FF);
}

void check_scroll(){
    int height=get_height();
    if((height-1)>ROWS){
   
    
        full_scroll();
        show_buffer();
       
   }
 
}

void show_buffer(){
   
    char *vid_mem = (char*)VIDEO_MEMORY;
    for(int i=0;i<ROWS*COLS*2;i+=2){
        vid_mem[i]=vid_buffer[i+DISPLAY_OFFSET];
        
        vid_mem[i+1]=vid_buffer[i+DISPLAY_OFFSET+1];
        
        
        
    }

     

    
}

void update_display(){
    show_buffer();
    check_scroll();

}


void write_to_cell(int cell,char c){
    if(scroll_pos==0){
       vid_buffer[cell+DISPLAY_OFFSET]=c;
    }else{
    vid_buffer[cell+scroll_pos]=c;
    }
   
}


void write_to_cell_c(int cell,char c,uint8_t color){
    if(scroll_pos==0){
       vid_buffer[cell+DISPLAY_OFFSET]=c;
       vid_buffer[cell+1+DISPLAY_OFFSET]=(color&0x0F)|(vid_buffer[cell+1+DISPLAY_OFFSET]&0xF0);
    }else{
    vid_buffer[scroll_pos+cell]=c;
     vid_buffer[scroll_pos+cell+1]=(color&0x0F)|(vid_buffer[scroll_pos+cell+1]&0xF0);
    }
   
}


int clamp(int a, int b, int c) {
    if (a > b) {
        return c;
    } else {
        return a;
    }
}


void m_cursor_move(){
      int cursor_pos=last_pos/2;      
      int row = (cursor_pos/COLS);
      row =clamp(row,(ROWS/2+12),(ROWS/2+12));
      int col = (cursor_pos%COLS);
      move_cursor(row*COLS+col);
}

int get_cursor_position(){
    int cursor_pos = last_pos/2;
    int row =(cursor_pos/COLS);
    row = clamp(row,(ROWS/2+12),(ROWS/2+12));
    int col = (cursor_pos%COLS);
    return row*COLS+col;
}

int cur_row(){
int cursor_pos = last_pos/2;
int row = (cursor_pos/COLS);
row=clamp(row,(ROWS/2+12),(ROWS/2+12));
return row;
}

int cur_col(){
      int cursor_pos = last_pos/2;
      int col = (cursor_pos%COLS);
      return col;
}

void get_line(char* str){
     for(uint32_t i=0;i<=buffer_input_index;i++){
        str[i]=buffer_input[i];
        buffer_input[i]='\0';
     }
     buffer_input_index=0;
    
}


void display_buff(const char *str){
      int i=0;

      int cursor_pos=0;
      while (str[i]!='\0')
      {
         if(str[i]=='\n'){
          last_pos = ((last_pos / (COLS*2)) + 1) * (COLS*2)-2;
       
      }else{
       write_to_cell(last_pos,str[i]);
      }      
      i++;
      last_pos+=2;
      cursor_pos=last_pos/2;      
      int row = (cursor_pos/COLS);
      row =clamp(row,(ROWS/2+12),(ROWS/2+12));
      int col = (cursor_pos%COLS);

      move_cursor(row*COLS+col);
      
}
}

void display_buff_c(const char *str,uint8_t color){
      int i=0;

      int cursor_pos=0;
      while (str[i]!='\0')
      {
         if(str[i]=='\n'){
          last_pos = ((last_pos / (COLS*2)) + 1) * (COLS*2)-2;
       
      }else{
       write_to_cell_c(last_pos,str[i],color);
      }      
      i++;
      last_pos+=2;
      cursor_pos=last_pos/2;      
      int row = (cursor_pos/COLS);
      row =clamp(row,(ROWS/2+12),(ROWS/2+12));
      int col = (cursor_pos%COLS);

      move_cursor(row*COLS+col);
      
}
}

void print(const char *str){
    printc(str,WHITE);
}

void printc(const char *str,uint8_t color){
    
    display_buff_c(str,color);
 

}

char keycode_to_char(uint8_t keycode) {
    
char key_code_lookup_table[] ={'\0','\0','1','2','3','4','5','6','7','8','9','0','-','+','\0','\0',
                 'q','w','e','r','t','y','u','i','o','p','{','}','\n','\0',
                 'a','s','d','f','g','h','j','k','l',';','\0','\0','\0','\\',
                 'z','x','c','v','b','n','m',',','.','/','\0','\0','\0',' '
                 };
   return key_code_lookup_table[keycode];
}


// Function to display the keycode

void print_char(char value){
    buffer_input[buffer_input_index++]=value;
    char str[2];
    str[0]=value;
    str[1]='\0';
    print(str);
}

void display_char(uint8_t keycode){
    char value = keycode_to_char(keycode);
    print_char(value);
    
}


// Keyboard ISR
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
    char** args = get_args(input,80);

    if(strcmp(input,"clear")==0){
        reset_shell();
        show_buffer();
        outb(PIC1_COMMAND, PIC_EOI); 
        return;
    }

    if(strcmp(args[0],"mkdir")==0){
       make_dir(args[1]);
       print("\n");
       printc("$p> ",GREEN);
       show_buffer();

       outb(PIC1_COMMAND, PIC_EOI);
       return;
    }

    if(strcmp(args[0],"ls")==0){
        list_all();
       
        printc("$p> ",GREEN);
         show_buffer();
        outb(PIC1_COMMAND, PIC_EOI);
        return;
    }

    if(strcmp(args[0],"chbg")==0){
        set_text_color(WHITE);
        if(strcmp(args[1],"red")==0){
            set_background(RED);
        }
        else if(strcmp(args[1],"green")==0){
            set_background(GREEN);
        }
        else if(strcmp(args[1],"blue")==0){
            set_background(BLUE);
        }
        else if(strcmp(args[1],"black")==0){
            set_background(BLACK);
        }else if(strcmp(args[1],"orange")==0){
            set_background(RED|GREEN);
        }else if(strcmp(args[1],"yellow")==0){
            set_background(RED|LIGHT_BROWN);
            set_text_color(BLACK);
        }else if(strcmp(args[1],"pink")==0){
            set_background(LIGHT_RED);
        }else if(strcmp(args[1],"gray")==0){
            set_background(LIGHT_GRAY);
            set_text_color(BLACK);
        }else if(strcmp(args[1],"magenta")==0){
            set_background(MAGENTA);
        }
        
        else{
            set_background(BLACK);
        }
        reset_shell();
        show_buffer();
        outb(PIC1_COMMAND,PIC_EOI);

        return;
    }
    print("\nUnknown command:");
    print(input);
    print("\n");
   

    
    printc("$p> ",GREEN);
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


void display_keycode(uint8_t keycode){
        char str[4];
        itoa(keycode,str);
        char message[] = "Keycode:";
        print(message);
        print(str);
}






// Function to enable keyboard interrupts
void enable_keyboard_interrupt() {
    uint8_t mask = inb(PIC1_DATA);
    mask &= ~(1 << 1); // Unmask IRQ1
    outb(PIC1_DATA, mask);
}

void init_pic() {
    // Initialize PIC1
    outb(0x20, 0x11); // Start initialization sequence
    outb(0x21, 0x20); // Set IRQ base (32)
    outb(0x21, 0x04); // Connect PIC1 to IRQ2 (cascade)
    outb(0x21, 0x01); // 8086/88 mode
    outb(0x21, 0xFF); // Mask all interrupts initially

    // Initialize PIC2 (if present, typically cascaded to PIC1)
    outb(0xA0, 0x11); // Start initialization sequence
    outb(0xA1, 0x28); // Set IRQ base (40)
    outb(0xA1, 0x02); // Connect PIC2 to IRQ2
    outb(0xA1, 0x01); // 8086/88 mode
    outb(0xA1, 0xFF); // Mask all interrupts initially
}


void set_text_color(unsigned char bg){
    text_color=bg;
    char *vid_mem = vid_buffer;
    uint32_t i=0;
    while (i<BUFFER_SIZE)
    {
       vid_mem[i+1]= (bg&0x0F)|(vid_mem[i+1]&0xF0);

       i+=2;
    }
    
}

void set_background(unsigned char fg){
    char *vid_mem = vid_buffer;
    background_color=fg;
    uint32_t i=0;
    while (i<BUFFER_SIZE)
    {
       vid_mem[i+1]=  (vid_mem[i+1]&0x0F)|(fg<<4);
    
       i+=2;
    }
}


void shift_buffer(int source, int destination, int depth) {
    // Calculate the number of bytes to shift based on depth
    int shift_size = depth * COLS *2;

  
    if (source < 0 || destination < 0 || 
        source + shift_size > BUFFER_SIZE || destination + shift_size > BUFFER_SIZE) {
      
        return;
    }

    if (source < destination) {
        // Copy backwards if the destination is higher than the source
        for (int i = shift_size-2; i >= 0; i -=2) {
            vid_buffer[destination + i] = vid_buffer[source + i];
            vid_buffer[destination+i+1]=vid_buffer[source+i+1];
            vid_buffer[source + i] = ' ';
            
        }
    } else {
        // Copy forwards if the source is higher than the destination or if they are the same
        for (int i = 0; i < shift_size; i +=2) {
            vid_buffer[destination + i] = vid_buffer[source + i];
            vid_buffer[destination+i+1]=vid_buffer[source+i+1];
            vid_buffer[source + i] = ' ';
            vid_buffer[source+i+1]=((background_color & 0x0F) << 4) | (text_color & 0x0F);
        }
    }
    
}





// void scroll_up(){
//     char* vid_mem = vid_buffer;
//     for(int j=0;j<ROWS;j++){
//     for(int i=0;i<COLS*2;i+=2){

//         int offset = i+j*COLS*2+2000;
//         int next_offset= i+(j+1)*COLS*2+2000;
//         vid_mem[offset]=vid_mem[next_offset];
//         vid_mem[next_offset]=' ';
       
//     }
//     }  
// }

// void scroll_down(){
//     char* vid_mem = vid_buffer;
//     for(int j=24;j>=0;j--){
//     int row=j;
//     int offset = row*COLS*2+2000;
//     int prev_offset=(row-1)*COLS*2+2000;
//     for(int i=0;i<COLS*2;i+=2){
//         char temp=vid_mem[prev_offset+i];
//         vid_mem[prev_offset+i]=' ';
//         vid_mem[offset+i]=temp;
//     }
//     }
// }

int get_height(){
    return ((last_pos/COLS)/2)+1;
}

void full_scroll(){
    
    int ht = get_height()-1;
    int st=0;
    int ers=ht-ROWS;
    

    int sm = (ers/scroll_size);
   

    st= sm*COLS*2;

    shift_buffer(DISPLAY_OFFSET,DISPLAY_OFFSET-st,ht);
    scroll_pos=DISPLAY_OFFSET-st;
    
    current_height=-sm;
    

}

void scroll_up(){
    int height = get_height();
    int shift=0;
    int extra_rows=height-ROWS;
    if(extra_rows<=0){
        return;
    }

    int scroll_amount = (extra_rows/scroll_size);
    if(scroll_amount<abs(current_height)+2){
        m_cursor_move();
    }
    if(abs(current_height)<scroll_amount){

    shift= scroll_size*COLS*2;
    if(scroll_pos==0){
    shift_buffer(DISPLAY_OFFSET,DISPLAY_OFFSET-shift,height);
    scroll_pos=DISPLAY_OFFSET-shift;
    }else{
        shift_buffer(scroll_pos,scroll_pos-shift,height);
        scroll_pos=scroll_pos-shift;
    }
    current_height--;
  
} 
  


}

void print_number(int num){
    char str[4];
    itoa(num,str);
    print(str);
}



void scroll_down(){
    if(current_height<0){
    int height = get_height();
    int shift=0;
    shift= scroll_size*COLS*2;
    if(scroll_pos==0){
    shift_buffer(DISPLAY_OFFSET-shift,DISPLAY_OFFSET,height);
    scroll_pos=DISPLAY_OFFSET;
    }else{
        shift_buffer(scroll_pos,scroll_pos+shift,height);
        scroll_pos=scroll_pos+shift;
    }
    move_cursor(scroll_pos+ROWS*COLS);
    current_height++;
    }
}



void print_hex(uint32_t num) {
    const char* hex_digits = "0123456789ABCDEF";
    char buffer[10]; // Enough for 8 hex digits + null terminator
    int i = 0;

    // Add prefix
    buffer[i++] = '0';
    buffer[i++] = 'x';

    // Convert to hexadecimal string
    for (int j = 28; j >= 0; j -= 4) {
        uint8_t hex_digit = (num >> j) & 0xF; // Get the current hex digit
        if (hex_digit || i > 2) { // Only print leading zeros if we haven't printed any significant digits yet
            buffer[i++] = hex_digits[hex_digit];
        }
    }
    
    // Null-terminate the string
    buffer[i] = '\0';

    // Print the complete hexadecimal string
    print(buffer);
}



