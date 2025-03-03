global read_key

read_key:
    mov eax, 3
    int 0x80
    ret