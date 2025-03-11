#ifndef MOUSE_H
#define MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <utils.h>
#include <stdbool.h>

void init_mouse();
void mouse_handler(struct InterruptRegisters *regs);
uint32_t getMouseX();
uint32_t getMouseY();
int8_t getMouseXDelta();
int8_t getMouseYDelta();
bool getLeftButtonPress();
void draw_cursor(uint32_t x,uint32_t y,uint8_t color);
void restore_background(int x, int y);
void save_background(int x, int y);
#ifdef __cplusplus
}
#endif
#endif