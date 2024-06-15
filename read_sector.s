section .data
    buffer_size equ 512     ; Define buffer size

section .bss
    buffer resb buffer_size ; Reserve buffer space for reading data

section .text
global read_sector

; Function to read data from a sector on the disk
; Inputs:
;   dl = drive number (e.g., 0x80 for the first hard disk)
;   ecx = sector number
; Output:
;   Returns a pointer to the read data
read_sector:
    pushad                  ; Save registers

    ; Set up disk command
    mov dx, 0x1F7          ; Disk command register
    mov al, 0x20           ; Read sector command
    out dx, al             ; Send read sector command to disk controller

    ; Set up disk sector count
    mov dx, 0x1F2          ; Disk sector count register
    mov al, 1              ; Number of sectors to read
    out dx, al             ; Send sector count to disk controller

    ; Set up disk sector number
    mov dx, 0x1F3          ; Disk sector number register
    mov eax, ecx           ; Move the sector number into EAX
    shl eax, 9             ; Multiply sector number by sector size (512 bytes)
    out dx, al             ; Send sector number to disk controller
    mov al, ah             ; Move the upper 8 bits of EAX into AL
    out dx, al             ; Send the upper 8 bits of the sector number to disk controller

    ; Set up disk cylinder low
    mov dx, 0x1F4          ; Disk cylinder low register
    mov eax, ecx           ; Move the sector number into EAX
    shr eax, 2             ; Divide sector number by 63 (sectors per cylinder)
    out dx, al             ; Send lower 8 bits of cylinder number to disk controller

    ; Set up disk cylinder high
    mov dx, 0x1F5          ; Disk cylinder high register
    mov eax, ecx           ; Move the sector number into EAX
    shr eax, 10            ; Divide sector number by 63 (sectors per cylinder)
    out dx, al             ; Send upper 8 bits of cylinder number to disk controller

    ; Set up disk head
    mov dx, 0x1F6          ; Disk head register
    mov al, 0              ; Head number (0 for CHS mode)
    out dx, al             ; Send head number to disk controller

    ; Read data from the disk
    mov ecx, buffer        ; Move the buffer address into ECX
    mov dx, 0x1F0          ; Disk data register
    mov eax, buffer_size   ; Number of bytes to read (sector size)
    rep insb               ; Read data from disk to buffer

    popad                   ; Restore registers

    ; Return pointer to the read data
    mov eax, buffer        ; Move the buffer address into EAX
    ret

