global start_process

section .bss
    stack_pointer resd 1
    instruction_pointer resd 1


section .text
start_process:
    mov eax,1
    mov ebx, [esp+4]
    mov ecx, [esp+8]
    int 0x80
    ret


global trigger_process

trigger_process:
    ; Save the return address (this is where the process will return after being triggered)
    mov eax,[esp+4]
    mov [instruction_pointer],eax   ; Get the address to jump to (this will be the user process' entry point)
    mov ebx,[esp+8]
    mov [stack_pointer],ebx
    ; 1. Switch to user data segments (Ring 3)
    mov ax, 0x23          ; User data segment (GDT entry: 0x23)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. Load user stack pointer
    mov esp, [stack_pointer]    ; Set ESP to the top of the user stack

    ; 3. Build IRET stack frame (order: SS, ESP, EFLAGS, CS, EIP)
    push 0x23              ; Push user stack segment (SS)
    push esp               ; Push user stack pointer (ESP)

    ; Set EFLAGS with IF=1 (enable interrupts)
    pushfd                 ; Save current flags
    pop eax                ; Modify EFLAGS
    or eax, 0x200          ; Set the Interrupt Flag (IF)
    push eax               ; Push the modified EFLAGS

    push 0x1B              ; User code segment (CS = 0x1B, Ring 3)
    mov eax,[instruction_pointer]
    push eax              ; Entry point (EIP) - user process entry point

    ; 4. Enable interrupts and switch to user mode
    sti                    ; Enable interrupts in kernel mode
    iret                   ; Perform the IRET, this will transfer to user mode
