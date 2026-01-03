#ifndef TIMER_H
#define TIMER_H
#include <utils.h>
void init_timer();
uint32_t onIRQ0(struct InterruptRegisters *regs);
#endif