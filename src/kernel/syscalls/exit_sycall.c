#include <idt.h>
#include <gdt.h>
#include <kernel/syscalls/exit_syscall.h>

void exit_handler(struct InterruptRegisters *regs){
    switch_to_user_mode();
  
}

void init_exit_syscall()
{
    install_syscall_handler(5,exit_handler);
}


