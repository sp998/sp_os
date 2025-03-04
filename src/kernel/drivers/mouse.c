#include <kernel/drivers/mouse.h>
#include <idt.h>
#include <io.h>
#include <kernel/drivers/vga.h>

#define MOUSE_COMMAND_PORT 0x64
#define MOUSE_DATA_PORT 0x60

#define SCREEN_WIDTH  320  // Adjust based on your resolution
#define SCREEN_HEIGHT 200  

uint8_t buffer[3];
uint8_t offset = 0;
uint8_t buttons = 0;

void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) { // Data available to read
        while (timeout--) {
            if (inPortB(MOUSE_COMMAND_PORT) & 1) return;
        }
    } else { // Ready to send data
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

void init_mouse() {
    // Enable the auxiliary device (mouse)
    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0xA8);

    // Enable interrupts
    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0x20);
    uint8_t status = inPortB(MOUSE_DATA_PORT) | 2;
    mouse_wait(1);
    outPortB(MOUSE_COMMAND_PORT, 0x60);
    mouse_wait(1);
    outPortB(MOUSE_DATA_PORT, status);

    // Set mouse defaults
    mouse_write(0xF6);
    mouse_read(); // Acknowledge

    // Enable the mouse
    mouse_write(0xF4);
    mouse_read(); // Acknowledge

    // Install the IRQ handler
    irq_install_handler(12, mouse_handler);
}

// Track mouse position
int mouse_x = SCREEN_WIDTH / 2;   // Start in center
int mouse_y = SCREEN_HEIGHT / 2;

// Function to draw a simple cursor
void draw_cursor(int x, int y, uint8_t color) {
    putPixelWithIndex(x, y, color);
    putPixelWithIndex(x + 1, y, color);
    putPixelWithIndex(x, y + 1, color);
    putPixelWithIndex(x + 1, y + 1, color);
}

// Function to clear the cursor from the screen
void erase_cursor(int x, int y) {
    putPixelWithIndex(x, y, 0x00);   // Set to background color (black)
    putPixelWithIndex(x + 1, y, 0x00);
    putPixelWithIndex(x, y + 1, 0x00);
    putPixelWithIndex(x + 1, y + 1, 0x00);
}

void mouse_handler(struct InterruptRegisters *regs) {
    uint8_t status = inPortB(MOUSE_COMMAND_PORT);
    if (!(status & 0x20)) return;

    buffer[offset] = inPortB(MOUSE_DATA_PORT);
    offset = (offset + 1) % 3;

    if (offset == 0) {
        // Erase old cursor
        erase_cursor(mouse_x, mouse_y);

        // Update position
        mouse_x += (int8_t)buffer[1]; // X movement
        mouse_y -= (int8_t)buffer[2]; // Y is inverted

        // Clamp values to prevent out-of-bounds movement
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x >= SCREEN_WIDTH) mouse_x = SCREEN_WIDTH - 1;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;

        // Draw new cursor
        draw_cursor(mouse_x, mouse_y, 0xE); // Yellow cursor

        // Update the display
        update_display();
    }
}