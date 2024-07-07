global gdt_flush

section .data
gdt:
    ; Null descriptor
    dd 0
    dd 0

    ; Code segment descriptor
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0x9A      ; Access: Present, Ring 0, Executable, Readable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

    ; Data segment descriptor
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0x92      ; Access: Present, Ring 0, Writable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

    ; Stack Segment descriptor
    dw 0xFFFF   ; Limit low
    dw 0x0000   ; Base low
    db 0x00     ; Base middle
    db 0x92      ; Access: Present, Ring 0, Writable
    db 0xCF     ; Granularity: 4 KB, 32-bit
    db 0x00     ; Base high

gdtr:
    dw gdt_end - gdt - 1  ; Limit
    dd gdt                ; Base

section .text
gdt_flush:
    lgdt [gdtr]

    mov eax, 0x10          ; Data segment selector
    mov ds, eax            ; Set data segment to point to the data segment
    mov es, eax            ; Set extra segment to point to the data segment
    mov fs, eax            ; Set FS segment to point to the data segment
    mov gs, eax            ; Set GS segment to point to the data segment

    mov eax, 0x18          ; Stack segment selector
    mov ss, eax            ; Set the stack segment register to point to the stack segment

    jmp 0x08:flush         ; Jump to the code segment

flush:
    mov eax, 0x10          ; Reload DS one more time
    mov ds, eax
    ret

gdt_end: