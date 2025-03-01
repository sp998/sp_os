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

# Find all source files recursively
C_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.c")
CXX_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
ASM_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.s")


# Convert source file paths to object file paths in BUILD_DIR while preserving directory structure
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
           $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CXX_SOURCES)) \
           $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

# Output files
ISO_FILE := os.iso
DISK_IMAGE := mydisk.img

# Build kernel
all: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Ensure object file directories exist before compiling
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
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
	./start_qemu.sh
# Download ISO to Windows (WSL)
download:
	cp $(ISO_FILE) /mnt/c/Users/Thinkpad/Downloads

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(ISO_FILE) $(DISK_IMAGE) $(ISO_DIR)/boot/kernel.elf

.PHONY: all os.iso run clean download create-disk flash
