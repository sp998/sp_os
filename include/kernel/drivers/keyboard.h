#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <utils.h>

void init_keyboard();
uint32_t keyboard_handler(struct InterruptRegisters *regs);
#endif