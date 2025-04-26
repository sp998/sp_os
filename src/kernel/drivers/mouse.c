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

int8_t mouse_x_delta;
int8_t mouse_y_delta;

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

void draw_cursor_with_offset(uint32_t x, uint32_t y, uint8_t color, int32_t tail_offset, int32_t tail_slope) {
    if (!cursor_initialized) {
        cursor_initialized = 1;
        return;
    }

    // Draw the classic arrow cursor shape
    // First row
    putPixelWithIndex(x, y, color);
    
    // Second row
    putPixelWithIndex(x, y + 1, color);
    putPixelWithIndex(x + 1, y + 1, color);
    
    // Third row
    putPixelWithIndex(x, y + 2, color);
    putPixelWithIndex(x + 1, y + 2, color);
    putPixelWithIndex(x + 2, y + 2, color);
    
    // Fourth row
    putPixelWithIndex(x, y + 3, color);
    putPixelWithIndex(x + 1, y + 3, color);
    putPixelWithIndex(x + 2, y + 3, color);
    putPixelWithIndex(x + 3, y + 3, color);
    
    // Fifth row
    putPixelWithIndex(x, y + 4, color);
    putPixelWithIndex(x + 1, y + 4, color);
    putPixelWithIndex(x + 2, y + 4, color);
    putPixelWithIndex(x + 3, y + 4, color);
    putPixelWithIndex(x + 4, y + 4, color);
    
    // Sixth row
    putPixelWithIndex(x, y + 5, color);
    putPixelWithIndex(x + 1, y + 5, color);
    putPixelWithIndex(x + 2, y + 5, color);
    putPixelWithIndex(x + 3, y + 5, color);
    putPixelWithIndex(x + 4, y + 5, color);
    putPixelWithIndex(x + 5, y + 5, color);
    
    // Seventh row - Tail begins
    putPixelWithIndex(x, y + 6, color);
    putPixelWithIndex(x + 1, y + 6, color);
    putPixelWithIndex(x + 2, y + 6, color);
    putPixelWithIndex(x + 3, y + 6, color);
    putPixelWithIndex(x + 6 + tail_offset, y + 6, color);
    putPixelWithIndex(x + 7 + tail_offset, y + 6, color);
    
    // Eighth row
    putPixelWithIndex(x, y + 7, color);
    putPixelWithIndex(x + 1, y + 7, color);
    putPixelWithIndex(x + 7 + tail_offset, y + 7 + tail_slope, color);
    putPixelWithIndex(x + 8 + tail_offset, y + 7 + tail_slope, color);
    
    // Ninth row
    putPixelWithIndex(x + 8 + tail_offset, y + 8 + tail_slope * 2, color);
    putPixelWithIndex(x + 9 + tail_offset, y + 8 + tail_slope * 2, color);
    
    // Tenth row
    putPixelWithIndex(x + 9 + tail_offset, y + 9 + tail_slope * 3, color);
    putPixelWithIndex(x + 10 + tail_offset, y + 9 + tail_slope * 3, color);
}

void draw_cursor(uint32_t x, uint32_t y, uint8_t color) {
   draw_cursor_with_offset(x,y,color,-4,1);
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

int8_t getMouseXDelta(){
return mouse_x_delta;
}
int8_t getMouseYDelta(){
 return -1*mouse_y_delta;
}


void mouse_handler(struct InterruptRegisters *regs) {
    uint8_t status = inPortB(MOUSE_COMMAND_PORT);
    if (!(status & 0x20)) return;

    buffer[offset] = inPortB(MOUSE_DATA_PORT);
    offset = (offset + 1) % 3;

    if (offset == 0) {
       

        left_button_pressed = (buffer[0] & 0x01) != 0;
      
            if(cursor_initialized){
            //restoreBackground();
            }
        
    

       
        mouse_x += (int8_t)buffer[1];
        mouse_y -= (int8_t)buffer[2];

        mouse_x_delta =(int8_t)buffer[1];
        mouse_y_delta = (int8_t)buffer[2];

     
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x >= SCREEN_WIDTH) mouse_x = SCREEN_WIDTH - 1;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;

       

        if (left_button_pressed) {
            
            //draw_cursor(mouse_x, mouse_y, 0x10); 
        } else {
         
            //draw_cursor(mouse_x, mouse_y, 0x10); 
        }

    }
}

uint32_t getMouseX()
{
    return mouse_x;
}

uint32_t getMouseY()
{
    return mouse_y;
}

bool getLeftButtonPress()
{
    return left_button_pressed;
}
