section .text
global get_drive_number

get_drive_number:
    ; Extract the drive number (integer) from $root
    mov esi, $root
    mov al, byte [esi + 4]  ; Byte at offset 4 contains the drive number (e.g., 0 for hd0, 1 for hd1, etc.)
    ; Now 'al' contains the numeric value representing the boot drive

    ; Return the drive number (in AL)
    ret

section .data
    ; Define the $root environment variable (for demonstration)
    ; Replace this with the actual value provided by GRUB
    $root db "(hd1)",0
