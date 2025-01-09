#ifndef ACHI_H
#define ACHI_H
#include <stdint.h>
#include <stdbool.h>

// Define AHCI controller registers
#define AHCI_BASE_ADDR          0x80000000   // Example base address of AHCI controller
#define AHCI_GLOBAL_CONTROL     (AHCI_BASE_ADDR + 0x04)
#define AHCI_CAPABILITIES       (AHCI_BASE_ADDR + 0x08)
#define AHCI_COMMAND_LIST_ADDR  (AHCI_BASE_ADDR + 0x18)
#define AHCI_COMMAND_TABLE_ADDR (AHCI_BASE_ADDR + 0x20)

// Define AHCI controller register bits
#define AHCI_GLOBAL_CONTROL_AHCI_ENABLE_BIT  (1 << 31)

// Function to initialize AHCI controller
bool ahci_controller_init();
bool ahci_write_disk(uint32_t sector, void* data, uint32_t sector_count);

#endif