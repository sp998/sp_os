KERNEL_STACK_SIZE equ  4096

global outb

global loader 
    
section .bss:
align 4
    kernel_stack:
        resb KERNEL_STACK_SIZE




section .text
align 4
loader: 
 
    extern kmain
    call kmain
    
    .loop:
        jmp .loop                   ; loop forever         ;loop forever

   outb:
        mov al,[esp+8]
        mov dx,[esp+4]
        out dx,al
        ret


global load_idt

load_idt:
    mov eax, [esp+4]
    lidt [eax]
    ret

global inb

inb:
    mov dx,[esp+4]
    in al,dx
    ret



global keyboard_isr 
keyboard_isr:
    extern keyboard_isr_handler
    cli
    pusha
    call keyboard_isr_handler
    popa
    sti
    iretd


      

