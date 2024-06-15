; disk.asm

global disk_init
global disk_write_sector
global ide_controller_init
section .text

; Function to initialize the disk controller
; No parameters
; Returns: Nothing
; Function to initialize the IDE disk controller
; No parameters
; Returns: Nothing

ide_controller_init:
    ; Initialize IDE controller
    mov dx, 0x1F6        ; Select IDE master device
    mov al, 0xA0
    out dx, al

    mov dx, 0x3F6        ; Control register
    mov al, 0x00         ; Disable interrupts, set device ready
    out dx, al

    ; Check IDE controller status
    in al, 0x1F7         ; Read status register
    test al, 0xC0        ; Check if error or device not present
    jnz ide_init_error   ; Jump to error handling if error or device not present

    ret

ide_init_error:
    ; Handle IDE controller initialization error
    ; You can implement error handling code here, such as retrying initialization or reporting an error
    extern disk_not_init
    call disk_not_init
    ret

disk_init:
    ; Wait for the disk to become ready
    wait_disk_ready:
        in al, 0x1F7       ; Read status register
        test al, 0x40      ; Check if the busy bit is set
        jnz wait_disk_ready  ; Wait until the disk is not busy

    ; Send device reset command
    mov dx, 0x1F7         ; Set the command register
    mov al, 0x04          ; Send DEVICE RESET command
    out dx, al

    ; Wait for the disk to become ready after reset
    wait_disk_ready_after_reset:
        in al, 0x1F7       ; Read status register
        test al, 0x40      ; Check if the busy bit is set
        jnz wait_disk_ready_after_reset  ; Wait until the disk is not busy

    ; Send identify command
    mov dx, 0x1F6         ; Set the disk/head register
    mov al, 0xA0          ; Select master device
    out dx, al

    ; Set up sector count register
    mov dx, 0x1F2         ; Set the sector count register
    xor ax, ax            ; Clear AX register
    out dx, al            ; Write to sector count register

    ; Set up LBA registers to read identification data
    mov dx, 0x1F3         ; Set the LBA low register
    mov al, 0x00          ; Send zero value for LBA low
    out dx, al

    mov dx, 0x1F4         ; Set the LBA mid register
    mov al, 0x00          ; Send zero value for LBA mid
    out dx, al

    mov dx, 0x1F5         ; Set the LBA high register
    mov al, 0x00          ; Send zero value for LBA high
    out dx, al

    ; Send IDENTIFY command
    mov dx, 0x1F7         ; Set the command register
    mov al, 0xEC          ; Send IDENTIFY command
    out dx, al

    ; Check if the command was accepted
    in al, 0x1F7          ; Read status register
    test al, 0x1          ; Check if the error bit is set
    jnz disk_init_error   ; Jump to error handling if error bit is set

    ; Read identification data from disk
    mov dx, 0x1F0         ; Set the data register
    mov cx, 256           ; Read 256 words (512 bytes) of identification data
    rep insw              ; Read data from disk into memory

    ret

disk_init_error:
    ; Error handling routine
    ; You can implement error handling code here, such as retrying initialization or reporting an error
    extern disk_not_init
    call disk_not_init
    ret


; Function to write data to a disk sector
; Parameters:
;   sector: Sector number to write to
;   data: Pointer to the data buffer
; Returns: Nothing
; Function to write data to a disk sector
; Parameters:
;   sector: Sector number to write to
;   data: Pointer to the data buffer
; Returns: Nothing
disk_write_sector:
    ; Calculate disk address from sector number
    mov eax, [ebp + 8]      ; Load sector number from function parameter
    mov edx, 0x1F6          ; Set the disk/head register
    mov cl, 6               ; Shift by 6 bits (sector size is 512 bytes)
    shl eax, cl             ; Shift sector number left by 6 bits
    mov cl, 8               ; Shift by 8 bits (word size)
    mov ebx, eax            ; Save the shifted sector number in ebx
    shr ebx, cl             ; Get the high 8 bits of LBA
    mov cl, 2               ; Shift by 2 bits (word size)
    mov esi, [ebp + 12]     ; Pointer to data buffer
    mov edi, eax            ; Save the shifted sector number in edi
    shr edi, cl             ; Get the mid 8 bits of LBA
    mov al, 0xE0            ; Set the disk/head register, select master device and LBA mode
    or al, bl               ; Set the top bit of LBA
    out dx, al              ; Send the disk/head register value to the port
    mov al, 1               ; Number of sectors to write
    out 0x1F2, al           ; Set the sector count register
    mov al, ah              ; Get the low 8 bits of LBA
    out 0x1F3, al           ; Set the LBA low register
    mov al, bl              ; Get the mid 8 bits of LBA
    out 0x1F4, al           ; Set the LBA mid register
    mov al, bh              ; Get the high 8 bits of LBA
    out 0x1F5, al           ; Set the LBA high register

    ; Wait for the disk to be ready after write
    wait_disk_ready_after_write:
        in al, 0x1F7       ; Read status register
        test al, 0x08      ; Check if the DRQ bit is set
        jz wait_disk_ready_after_write  ; Wait until data transfer is complete

    ; Send write sector command
    mov dx, 0x1F7         ; Set the command register
    mov al, 0x30          ; Send WRITE SECTORS command
    out dx, al

    ; Write data to disk
    mov ecx, 256          ; Number of words to write (512 bytes / 2)
    rep outsw             ; Write the data to disk using rep outsw instruction

    ret
