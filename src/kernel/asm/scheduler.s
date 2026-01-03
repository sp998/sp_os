section .text
global switch_task

; void switch_task(uint32_t next_esp, uint32_t next_cr3);
switch_task:
    ; Standard prologue not strictly needed for a naked switch, but we are called from C/Rust.
    ; Arguments are on stack: [esp+4] = next_esp, [esp+8] = next_cr3
    
    ; We do NOT need to save EIP/ESP of current task here because 
    ; the *caller* (rust_schedule -> timer_handler -> common_stub) 
    ; has already pushed registers and the current ESP is passed to rust_schedule.
    ; Wait, no. `rust_schedule` returns the *new* ESP.
    ; The Timer Handler (ASM) should assume EAX holds new ESP.
    
    ; Actually, looking at previous implementation/logic:
    ; `rust_schedule` returns the ESP.
    ; If we use `switch_task` inside rust code, we are switching *stacks*.
    
    ; However, for a preemptive scheduler hooked into IRQ:
    ; 1. IRQ fires -> CPU pushes EFLAGS, CS, EIP.
    ; 2. `common_stub` pushes GS, FS, ES, DS, EAX...EDI (Pusha).
    ; 3. `esp` points to `InterruptRegisters` structure.
    ; 4. We call `rust_schedule(esp)`.
    ; 5. `rust_schedule` updates the *current process* with that `esp`.
    ; 6. `rust_schedule` returns the `esp` of the *next* process.
    ; 7. `onIRQ0` usually returns void.
    ;    If we want to switch, `onIRQ0` needs to change the stack pointer before returning?
    
    ; Alternative: `switch_task` is used for `yield` or synchronous switch.
    ; But here we are integrating into `timer.c`.
    
    ; Let's re-implement `switch_task` for manual switching if needed (spawn), 
    ; but primarily relying on the IRQ stub return trick is better for preemption.
    
    ; BUT, to keep it simple and consistent with previous attempt:
    ; We used `switch_task` defined in ASM.
    
    mov eax, [esp + 4]      ; Get next_esp
    mov ecx, [esp + 8]      ; Get next_cr3
    
    ; Load new Page Directory (if different)
    mov edx, cr3
    cmp edx, ecx
    je .load_stack
    mov cr3, ecx
    
.load_stack:
    mov esp, eax            ; Switch Stack Pointer
    
    ; Now we are on the new stack. 
    ; Assuming this stack looks like it was "just pushed" by pusha/ISR or initialized similarly.
    
    ; If we are calling this from C, we expect to return.
    ; But if we switched stacks, 'ret' will pop from NEW stack.
    ; So the new stack must have a return address at top?
    ; Or if it was saved during an interrupt, it has `regs` popped eventually.
    
    ret
