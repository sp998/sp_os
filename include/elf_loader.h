#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ide.h>
#include <printf.h>
#include <utils.h>
#include <kernel/elf32.h>


#define USER_ELF_LOAD_ADDR 0x300000
#define SECTOR_SIZE 512  // Sector size is typically 512 bytes
#define ELF_FILE_SIZE 4628  // Size of ELF file in bytes (4.6 KB)
#define ELF_SECTOR_COUNT ((ELF_FILE_SIZE + SECTOR_SIZE - 1) / SECTOR_SIZE)  // Number of sectors required

// Function to load the ELF file into memory
bool load_elf_from_disk(uint32_t start_sector, void* buffer);
uint32_t load_and_print_elf(uint32_t start_sector);

#endif
