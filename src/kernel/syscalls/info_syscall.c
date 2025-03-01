#include <idt.h>
#include <utils.h>
#include <io.h>

void info_syscall_handler(struct InterruptRegisters* regs){
    print("System call occurred!");
    update_display();
}

void init_syscall_info(){
    install_syscall_handler(2,info_syscall_handler);
}