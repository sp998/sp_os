#include <kernel/syscalls/read_key.h>
#include <idt.h>
#include <sysvars.h>

void read_key_handler(struct InterruptRegisters *regs){
    __asm__ volatile("sti");
    while (!get_key_pressed())
    {
    }

    regs->eax=get_current_key();
    set_key_pressed(false);
  
}

void init_syscall_read_key(){
    install_syscall_handler(3,read_key_handler);
}