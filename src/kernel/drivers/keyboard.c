#include <kernel/drivers/keyboard.h>
#include <idt.h>


void init_keyboard()
{
   irq_install_handler(1, keyboard_handler);

}
void keyboard_handler(struct InterruptRegisters *regs)
{
    char scancode = inPortB(0x60)&0x7F;
    char pressed = inPortB(0x60)&0x80;
    print("code:");
    print_number(scancode);
    print(" pressed:");
    print_number(pressed);
    print("\n");
    update_display();
}