global start_process


section .text
start_process:
    mov eax,1
    mov ebx, [esp+4]
    mov ecx, [esp+8]
    int 0x80
    ret


global trigger_process

trigger_process:
    mov eax, [esp + 4]   
    jmp eax
    ret
