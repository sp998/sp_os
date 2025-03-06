#include <kernel/drivers/mouse.h>
#include <idt.h>
#include <io.h>
#include <kernel/drivers/vga.h>

#define MOUSE_COMMAND_PORT 0x64
#define MOUSE_DATA_PORT 0x60

#define SCREEN_WIDTH  320  
#define SCREEN_HEIGHT 200  

uint8_t buffer[3];
uint8_t offset = 0;
uint8_t buttons = 0;

int mouse_x = SCREEN_WIDTH / 2;
int mouse_y = SCREEN_HEIGHT / 2;

uint8_t saved_bg[25];  
int cursor_initialized = 0; 


int left_button_pressed = 0;


void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) { 
        while (timeout--) {
            if (inPortB(MOUSE_COMMAND_PORT) & 1) return;
        }
    } else { 
        while (timeout--) {
            if (!(inPortB(MOUSE_COMMAND_PORT) & 2)) return;
        }
    }
}


void mouse_write(uint8_t value) {
    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0xD4);
    mouse_wait(1);
    outPortB(MOUSE_DATA_PORT, value);
}


uint8_t mouse_read() {
    mouse_wait(0);
    return inPortB(MOUSE_DATA_PORT);
}


void save_background(int x, int y) {

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            saved_bg[i * 5 + j] = getPixelColor(x + j, y + i);
        }
    }
}

void restore_background(int x, int y) {

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            putPixelWithIndex(x + j, y + i, saved_bg[i * 5 + j]);
        }
    }
}


void draw_cursor(int x, int y, uint8_t color) {
    if (!cursor_initialized) {
      
        save_background(x, y);
        cursor_initialized=1;
        return;
       
    }

    save_background(x, y);

  
    putPixelWithIndex(x + 2, y, color);           
    putPixelWithIndex(x + 1, y + 1, color);      
    putPixelWithIndex(x + 2, y + 1, color);       
    putPixelWithIndex(x + 3, y + 1, color);      
    putPixelWithIndex(x, y + 2, color);           
    putPixelWithIndex(x + 1, y + 2, color);      
    putPixelWithIndex(x + 2, y + 2, color);       
    putPixelWithIndex(x + 3, y + 2, color);       
    putPixelWithIndex(x + 4, y + 2, color);      
    putPixelWithIndex(x + 1, y + 3, color);      
    putPixelWithIndex(x + 3, y + 3, color);       
    putPixelWithIndex(x + 2, y + 4, color);      

   
    update_display();
}


void init_mouse() {

    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0xA8);

    
    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0x20);
    uint8_t status = inPortB(MOUSE_DATA_PORT) | 2;
    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0x60);
    mouse_wait(1);
    outPortB(MOUSE_DATA_PORT, status);

 
    mouse_write(0xF6);
    mouse_read();

  
    mouse_write(0xF4);
    mouse_read(); 

  
    irq_install_handler(12, mouse_handler);
}


void mouse_handler(struct InterruptRegisters *regs) {
    uint8_t status = inPortB(MOUSE_COMMAND_PORT);
    if (!(status & 0x20)) return;

    buffer[offset] = inPortB(MOUSE_DATA_PORT);
    offset = (offset + 1) % 3;

    if (offset == 0) {
       

        left_button_pressed = (buffer[0] & 0x01) != 0;
        if(!left_button_pressed){
            if(cursor_initialized){
            restore_background(mouse_x, mouse_y);
            }
        }
    

       
        mouse_x += (int8_t)buffer[1];
        mouse_y -= (int8_t)buffer[2];

     
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x >= SCREEN_WIDTH) mouse_x = SCREEN_WIDTH - 1;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;

       

        if (left_button_pressed) {
            
            draw_cursor(mouse_x, mouse_y, 0x10); 
        } else {
         
            draw_cursor(mouse_x, mouse_y, 0x10); 
        }

        update_display();
    }
}
