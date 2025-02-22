#include <io.h>
#include <idt.h>
#include <timer.h>

#define TIMER_FREQ 1000


static uint32_t ticks = 0;
void onIRQ0(struct InterruptRegisters *regs)
{
    ticks++;
    printc("Timer Ticked",WHITE);
    update_display();
   
}
void init_timer()
{
    ticks = 0;
    irq_install_handler(0, &onIRQ0);

    uint32_t divisor = 1193180 / TIMER_FREQ;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)divisor & 0xFF);
    outb(0x40, (uint8_t)(divisor >> 8) & 0xFF);
}


