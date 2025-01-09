# Use a base image with minimal setup, we will install necessary tools
FROM debian:bullseye-slim

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    qemu \
    grub-pc-bin \
    xorriso \
    make \
    gcc-multilib \
    git \
    curl \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Set up environment variables
ENV CC=i386-elf-gcc
ENV CFLAGS="-m32 -ffreestanding -fno-builtin -fno-stack-protector -Iinclude -std=gnu99"
ENV LDFLAGS="-m elf_i386 -nostdlib -T link.ld"
ENV AS=nasm
ENV ASFLAGS="-f elf32"

# Set up working directory inside the container
WORKDIR /workspace

# Copy the entire project into the container
COPY . /workspace

# Build the kernel (this will create os.iso inside the container's workspace)
RUN make os.iso

# Set the default command to bash

