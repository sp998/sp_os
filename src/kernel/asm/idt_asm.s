global idt_flush

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret


%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        cli
        push dword 0
        push dword %1
        jmp isr_common_stub
%endmacro


%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        cli
        push dword %1
        jmp isr_common_stub
%endmacro

%macro IRQ 2
    global irq%1
    irq%1:
        cli
        push dword 0
        push dword %2
        jmp irq_common_stub
%endmacro


ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

ISR_NOERRCODE 128
ISR_NOERRCODE 177

IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47 



extern rust_schedule

global irq0
irq0:
    cli
    push 0          ; error code
    push 32         ; int_no

    ; Save context
    pusha           ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov eax, ds
    push eax
    mov eax, es
    push eax
    mov eax, fs
    push eax
    mov eax, gs
    push eax

    ; Switch to kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Pass current ESP (which points to saved GS) to rust_schedule
    push esp
    call rust_schedule
    ; rust_schedule returns the new ESP in EAX
    mov esp, eax

    ; Send EOI to PIC (Master PIC command port 0x20)
    mov al, 0x20
    out 0x20, al

    ; Restore context
    pop eax
    mov gs, ax
    pop eax
    mov fs, ax
    pop eax
    mov es, ax
    pop eax
    mov ds, ax

    popa
    add esp, 8      ; Pop int_no and error code
    iret

extern isr_handler
isr_common_stub:
    pusha
    
    mov eax, ds
    push eax
    mov eax, es
    push eax
    mov eax, fs
    push eax
    mov eax, gs
    push eax

    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax

    push esp ; point to InterruptRegisters

    call isr_handler
    
    ; isr_handler should return (uint32_t)regs
    mov esp, eax

    pop eax
    mov gs,ax
    pop eax
    mov fs,ax
    pop eax
    mov es,ax
    pop eax
    mov ds,ax

    popa
    add esp,8
    iret


extern irq_handler
irq_common_stub:
    pusha
    
    mov eax, ds
    push eax
    mov eax, es
    push eax
    mov eax, fs
    push eax
    mov eax, gs
    push eax

    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax

    push esp ; point to InterruptRegisters

    call irq_handler
    
    ; irq_handler returns (uint32_t)regs or new ESP
    mov esp, eax

    pop eax
    mov gs,ax
    pop eax
    mov fs,ax
    pop eax
    mov es,ax
    pop eax
    mov ds,ax

    popa
    add esp,8
    iret
