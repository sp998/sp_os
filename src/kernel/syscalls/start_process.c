#include <kernel/syscalls/start_process.h>
#include <io.h>
#include <idt.h>

extern void trigger_process(uint32_t eip, uint32_t esp);

void handler_start_process(struct InterruptRegisters *regs){

    //print("System call: Starting new process:\n");
    trigger_process(regs->ebx,regs->ecx);
}


void init_syscall_start_process()
{
    install_syscall_handler(1,handler_start_process);
}
