global gdt_flush

section .data
gdt:
    ; Null descriptor
    dd 0
    dd 0

    ; Kernel Code Segment Descriptor (Ring 0)
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0x9A     ; Access: Present, Ring 0, Executable, Readable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

    ; Kernel Data Segment Descriptor (Ring 0)
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0x92     ; Access: Present, Ring 0, Writable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

    ; Stack Segment Descriptor for Kernel (Ring 0)
    dw 0xFFFF   ; Limit low (0xFFFF = 65535 bytes)
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0x92     ; Access: Present, Ring 0, Writable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

    ; User Code Segment Descriptor (Ring 3)
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0xFA     ; Access: Present, Ring 3, Executable, Readable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

    ; User Data Segment Descriptor (Ring 3)
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0xF2     ; Access: Present, Ring 3, Writable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

gdtr:
    dw gdt_end - gdt - 1  ; Limit
    dd gdt                ; Base

section .text
gdt_flush:
    lgdt [gdtr]            ; Load the new GDT

    ; Reload segment registers for Kernel
    mov eax, 0x10          ; Kernel data segment selector
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    mov eax, 0x18          ; Kernel stack segment selector
    mov ss, eax

    ; Jump to Kernel code segment
    jmp 0x08:flush

flush:
    mov eax, 0x10          ; Reload DS one more time
    mov ds, eax
    ret

gdt_end:



global switch_to_usermode

section .text
switch_to_usermode:
    ; Set up the stack for user mode
    cli                     ; Disable interrupts during setup
    mov ax, 0x1B            ; User data segment selector (Ring 3, index 3 in GDT)
    mov ds, ax              ; Set DS to user data segment
    mov es, ax              ; Set ES to user data segment
    mov fs, ax              ; Set FS to user data segment
    mov gs, ax              ; Set GS to user data segment

    ; Set up the stack pointer for user mode
    mov eax, user_stack     ; User stack address (defined in .data or .bss)
    mov ss, ax              ; Load user stack segment selector (Ring 3)
    mov esp, eax            ; Load user stack pointer

    ; Push the values to simulate an interrupt return
    push 0x23               ; User data segment selector (Ring 3, index 4 in GDT)
    push eax                ; User stack pointer
    pushf                   ; Push EFLAGS
    pop eax                 ; Modify EFLAGS to enable interrupts in user mode
    or eax, 0x200           ; Set the IF flag
    push eax                ; Push the modified EFLAGS
    push 0x1B               ; User code segment selector (Ring 3, index 3 in GDT)
    push user_entry_point   ; User mode entry point

    iretd                   ; Return to user mode (IRET for 32-bit)

user_stack:
    resb 4096               ; Reserve 4 KB for user stack

user_entry_point:
    ; Placeholder for user mode entry point
    ; This label should point to your user-space program or code
    nop                     ; No operation, replace with actual user-mode logic
