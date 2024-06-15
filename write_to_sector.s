section .text
global write_to_disk

write_to_disk:
    ; Inputs:
    ;   - edx: Disk number (0 for hd0, 1 for hd1, etc.)
    ;   - ecx: LBA (Logical Block Address) where data should be written
    ;   - esi: Pointer to the buffer containing data to write
    ;   - edi: Number of sectors to write
    ; Outputs: None (data is written to disk)

    ; Setup disk parameters
    mov ah, 0x42          ; AH = 0x42 for extended disk access
    mov dl, byte [edx]    ; Disk number (0 for hd0, 1 for hd1, etc.)
    mov dh, 0             ; Head number (usually 0 for IDE)
    mov bx, word [ecx]    ; Cylinder and sector (LBA low 2 bytes)
    mov cx, word [ecx + 2]; Cylinder (LBA high 2 bytes)

    ; Setup data transfer parameters
    mov es, esi           ; ES points to the buffer containing data
    mov bx, 0             ; Offset within ES (0 for linear buffer)
    mov ah, 0x3           ; AH = 0x3 for write sectors
    mov al, byte [edi]    ; Number of sectors to write

    ; Call BIOS interrupt for disk I/O
    int 0x13              ; BIOS interrupt for disk I/O

    ; Check carry flag to detect errors
    jc error              ; If carry flag is set, an error occurred

    ret                   ; Return from the function

error:
    ; Handle error condition
    ; You can implement error handling logic here
    ; For example, print an error message, retry the operation, etc.
    ; You can use BIOS functions or implement your own error handling routines
    ret
