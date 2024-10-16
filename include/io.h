#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdint.h>

#define BLACK  (unsigned char) 0
#define BLUE (unsigned char) 1
#define GREEN (unsigned char) 2
#define CYAN (unsigned char) 3
#define RED (unsigned char) 4
#define MAGENTA (unsigned char) 5
#define BROWN (unsigned char) 6
#define LIGHT_GRAY (unsigned char) 7
#define DARK_GRAY (unsigned char)  8
#define LIGHT_BLUE (unsigned char) 9
#define LIGHT_GREEN (unsigned char) 10
#define LIGHT_CYAN (unsigned char) 11
#define LIGHT_RED (unsigned char) 12
#define LIGHT_MAGENTA (unsigned char) 13
#define LIGHT_BROWN (unsigned char) 14
#define WHITE (unsigned char) 15



 //IDT entry structure
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void init_pic();
void enable_keyboard_interrupt();
void clear_screen();
void display_keycode(uint8_t keycode);
void move_cursor(unsigned short pos);
void clear_screen();
void show_buffer();
void check_scroll();

void update_display();
void write_to_cell(int cell,char c);

void set_background(unsigned char bg);
void set_text_color(unsigned char fg);




// Function prototypes
extern void load_idt(uint32_t);
extern void keyboard_isr();
extern void enable_interrupts();
void init_idt();


void print(const char* str);
void print_hex(uint32_t num);
void move_cursor(unsigned short pos);
void write_to_cell(int cell,char c);
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t data);
void backspace();
void scroll_up();
void scroll_down();
void print_number(int num);
int get_height();
void print_char(char value);
void reset_shell();
void printc(const char *str,uint8_t color);
void write_to_cell_c(int cell,char c,uint8_t color);



#endif