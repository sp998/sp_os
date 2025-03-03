#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <utils.h>
#include <stdbool.h>

#define WIDTH 320
#define HEIGHT 200

#define MISC_PORT  0x3c2
#define CRTC_INDEX_PORT 0x3d4
#define CRTC_DATA_PORT 0x3d5
#define SEQUENCER_INDEX_PORT 0x3c4
#define SEQUENCER_DATA_PORT 0x3c5

#define GRAPHICS_CONTROLLER_INDEX_PORT 0x3ce
#define GRAPHICS_CONTROLLER_DATA_PORT 0x3cf

#define ATTRIBUTE_CONTROLLER_INDEX_PORT 0x3c0
#define ATTRIBUTE_CONTROLLER_READ_PORT  0x3c1
#define ATTRIBUTE_CONTROLLER_WRITE_PORT 0x3c0
#define ATTRIBUTE_CONTROLLER_RESET_PORT 0x3da


void writeRegisters(uint8_t* registers);
uint8_t* getFrameBufferSegment();

bool supportsMode(uint32_t width, uint32_t height, uint32_t colorDepth);
bool setVgaMode(uint32_t width,uint32_t height, uint32_t colorDepth);
void putPixel(uint32_t x, uint32_t y,uint8_t r,uint8_t g,uint8_t b);

#endif