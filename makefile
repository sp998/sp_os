CC = gcc
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -c -Iinclude
LDFLAGS = -m32 -nostdlib -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

# -Wall -Wextra -Werror 

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
ISO_DIR := iso

# Find all source files and generate object file names
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES := $(wildcard $(SRC_DIR)/*.s)
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o))

# ISO file and disk image names
ISO_FILE := os.iso
DISK_IMAGE := mydisk.img

all: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(AS) $(ASFLAGS) $< -o $@

os.iso: $(BUILD_DIR)/kernel.elf
	mkdir -p $(ISO_DIR)/boot
	cp $(BUILD_DIR)/kernel.elf $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

create-disk:
	qemu-img create -f raw $(DISK_IMAGE) 1G

# Flash the ISO to the disk image
flash: os.iso
	dd if=os.iso of=$(DISK_IMAGE) bs=4M conv=notrunc status=progress
	qemu-img resize $(DISK_IMAGE) 1G



# Run the OS in QEMU
run: create-disk os.iso flash
	qemu-system-i386 -usb -device usb-ehci,id=ehci -device usb-storage,drive=usb-drive -drive id=usb-drive,file=mydisk.img,format=raw,if=none -m 512M



download:
	cp $(ISO_FILE) /mnt/c/Users/Thinkpad/Downloads

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/kernel.elf $(ISO_FILE) $(DISK_IMAGE) $(ISO_DIR)/boot/kernel.elf

.PHONY: all os.iso run clean download create-disk flash
