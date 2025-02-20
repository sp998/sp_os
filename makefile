# Compiler settings
CC = gcc
CXX = g++
AS = nasm
LD = ld

# Compilation flags
CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -c -Iinclude
CXXFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector -fno-exceptions -fno-rtti -c -Iinclude 
ASFLAGS = -f elf
LDFLAGS = -m32 -nostdlib -T link.ld -melf_i386

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
ISO_DIR := iso

# Source files
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
CXX_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
ASM_SOURCES := $(wildcard $(SRC_DIR)/*.s)

# Object files
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, \
    $(C_SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o) $(ASM_SOURCES:.s=.o))

# Output files
ISO_FILE := os.iso
DISK_IMAGE := mydisk.img

# Build kernel
all: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Compilation rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(AS) $(ASFLAGS) $< -o $@

# Create ISO
os.iso: $(BUILD_DIR)/kernel.elf
	mkdir -p $(ISO_DIR)/boot
	cp $(BUILD_DIR)/kernel.elf $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

# Docker build environment
build-os:
	docker build -t os-dev-env .
	docker run -it --rm -v .:/workspace os-dev-env make os.iso

# Disk image creation
create-disk:
	qemu-img create -f raw $(DISK_IMAGE) 1G

# Flash OS to disk image
flash: os.iso
	dd if=os.iso of=$(DISK_IMAGE) bs=4M conv=notrunc status=progress
	qemu-img resize $(DISK_IMAGE) 1G

# Run OS in QEMU
run: build-os
	qemu-system-i386 -m 512M -cdrom os.iso

# Download ISO to Windows (WSL)
download:
	cp $(ISO_FILE) /mnt/c/Users/Thinkpad/Downloads

# Clean build files
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/kernel.elf $(ISO_FILE) $(DISK_IMAGE) $(ISO_DIR)/boot/kernel.elf

.PHONY: all os.iso run clean download create-disk flash
