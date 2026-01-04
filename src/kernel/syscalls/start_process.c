#include <kernel/syscalls/start_process.h>
#include <io.h>
#include <idt.h>

// Use the Rust scheduler to spawn process instead of just jumping to it
// This ensures the process gets its own stack and is managed by the scheduler
extern void rust_spawn_process(uint32_t entry_point, uint32_t stack_top);

void handler_start_process(struct InterruptRegisters *regs){

    //print("System call: Starting new process:\n");
    // Pass 0 as stack_top to force allocation of a new user stack
    rust_spawn_process(regs->ebx, 0);
}


void init_syscall_start_process()
{
    install_syscall_handler(1,handler_start_process);
}
