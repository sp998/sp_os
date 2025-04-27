#include <idt.h>
#include <gdt.h>
#include <kernel/syscalls/exit_syscall.h>
#include <sysvars.h>

void input_handler(struct InterruptRegisters *regs){
    set_user_input_mode(true);
    set_user_input_buffer((char*)regs->ebx);
    __asm__ volatile("sti");
    uint16_t original_offset= get_back_space_offset();
    set_back_space_offset(1);
    while(get_user_input_mode()){}
    set_back_space_offset(original_offset);
  
}

void init_input_syscall()
{
    install_syscall_handler(6,input_handler);
}