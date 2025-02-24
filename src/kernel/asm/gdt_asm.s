global gdt_flush


section .text
gdt_flush:
    mov eax,[esp+4]
    lgdt [eax]           ; Load the new GDT
    
    mov eax, 0x10          ; Kernel data segment selector
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss,eax

    ; Jump to Kernel code segment
    jmp 0x08:flush

flush:
    ret


global tss_flush

tss_flush:
    mov eax,0x2B
    ltr ax
    ret