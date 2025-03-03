section .multiboot_header
align 4
dd 0x1BADB002   ; Magic number
dd 0x00000003   ; Flags
dd -(0x1BADB002 + 0x00000003)   ; Checksum (must add up to 0 when summed with magic and flags)

