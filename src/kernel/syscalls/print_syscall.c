#include <kernel/syscalls/print_syscall.h>
#include <idt.h>
#include <io.h>

void print_syscall_handler(struct InterruptRegisters *regs){
    char *str = (char*)regs->ebx;
    print(str);
    update_display();

}

void init_syscall_print()
{
    install_syscall_handler(2,print_syscall_handler);
}