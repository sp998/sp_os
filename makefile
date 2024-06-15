CC = gcc
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -c
LDFLAGS = -m32 -nostdlib -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

# Find all source files and generate object file names
C_SOURCES = $(wildcard *.c)
ASM_SOURCES = $(wildcard *.s)
OBJECTS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

# os.iso: kernel.elf
# 	cp kernel.elf iso/boot/kernel.elf
# 	xorriso -as mkisofs \
#     -R -J -joliet-long \
#     -b boot/grub/stage2_eltorito \
#     -no-emul-boot \
#     -boot-load-size 4 \
#     -boot-info-table \
#     --eltorito-alt-boot \
#     --efi-boot boot/grub/efi.img \
#     --efi-boot-image \
#     --protective-msdos-label \
#     -input-charset utf-8 \
#     -o os.iso \
#     iso


os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	grub-mkrescue -o os.iso iso


run: os.iso
	qemu-system-i386 -boot d -cdrom os.iso -drive file=mydisk.img,format=raw -m 1024




 

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

download:
	cp os.iso /mnt/c/Users/Thinkpad/Downloads

clean:
	rm -rf *.o kernel.elf os.iso

.PHONY: all os.iso run clean download


