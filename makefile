CC = gcc
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -c -Iinclude
LDFLAGS = -m32 -nostdlib -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
ISO_DIR := iso

# Find all source files and generate object file names
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES := $(wildcard $(SRC_DIR)/*.s)
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o))

all: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(AS) $(ASFLAGS) $< -o $@

os.iso: $(BUILD_DIR)/kernel.elf
	cp $(BUILD_DIR)/kernel.elf $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o os.iso $(ISO_DIR)

run: os.iso
	qemu-system-i386 -boot d -cdrom os.iso -drive file=mydisk.img,format=raw -m 1024

download:
	cp os.iso /mnt/c/Users/Thinkpad/Downloads

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/kernel.elf os.iso

.PHONY: all os.iso run clean download
