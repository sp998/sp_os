#ifndef TIMER_H
#define TIMER_H
#include <utils.h>
void init_timer();
void onIRQ0(struct InterruptRegisters *regs);
#endif