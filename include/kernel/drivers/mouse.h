#ifndef MOUSE_H
#define MOUSE_H
#include <utils.h>

void init_mouse();
void mouse_handler(struct InterruptRegisters *regs);
#endif