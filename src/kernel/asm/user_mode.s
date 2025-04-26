global switch_to_user_mode
extern user_main



section .bss
    align 4
    user_stack:
        resb 32768    ; Allocate 32KB for user stack
    user_stack_top:



section .text
switch_to_user_mode:
    ; 1. Switch to user data segments (Ring 3)
    mov ax, 0x23        ; User data segment (GDT entry: 0x23)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. Load user stack pointer (critical for interrupts)
    mov esp, user_stack_top

    ; 3. Build IRET stack frame (order: SS, ESP, EFLAGS, CS, EIP)
    push 0x23           ; User stack segment (SS)
    push esp            ; User stack pointer (ESP)

    ; Set EFLAGS with IF=1 (enable interrupts)
    pushfd              ; Push current EFLAGS
    pop eax             ; Modify EFLAGS
    or eax, 0x200       ; Set bit 9 (IF)
    push eax            ; Push modified EFLAGS

    push 0x1B           ; User code segment (CS = 0x1B, Ring 3)
    push user_main      ; Entry point (EIP)

    ; 4. Enable interrupts and switch to user mode
    sti                 ; Enable interrupts in kernel mode
    iret                ; Jump to user mode






global make_sys_call
make_sys_call:
    mov eax, 2
    int 0x80
    ret
