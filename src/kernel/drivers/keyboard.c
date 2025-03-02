#include <kernel/drivers/keyboard.h>
#include <idt.h>
#include <stdbool.h>


void init_keyboard()
{
   irq_install_handler(1, keyboard_handler);

}
void keyboard_handler(struct InterruptRegisters *regs)
{   char rawcode = inPortB(0x60);
    char scancode =rawcode&0x7F;
    bool pressed = (rawcode & 0x80) == 0; 
    print("code:");
    print_number(scancode);
    print(" pressed:");
    print_number(pressed);
    print("\n");
    update_display();
}