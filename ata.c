#include "ata.h"
#include <stdint.h>
#include <stdio.h>

// Define PCI configuration space registers
#define PCI_CONFIG_VENDOR_ID    0x00
#define PCI_CONFIG_CLASS        0x0B

// Define IDE controller class and subclass codes
#define PCI_CLASS_MASS_STORAGE  0x01
#define PCI_SUBCLASS_IDE        0x01

// Define programming interface byte (Prog If) bits
#define IDE_PROG_IF_PRIMARY_NATIVE_MODE    (1 << 0)
#define IDE_PROG_IF_PRIMARY_MODE_SWITCH    (1 << 1)
#define IDE_PROG_IF_SECONDARY_NATIVE_MODE  (1 << 2)
#define IDE_PROG_IF_SECONDARY_MODE_SWITCH  (1 << 3)
#define IDE_PROG_IF_BUS_MASTER             (1 << 7)

// Define Base Address Registers (BARs)
#define PCI_BAR0    0x10
#define PCI_BAR1    0x14
#define PCI_BAR2    0x18
#define PCI_BAR3    0x1C
#define PCI_BAR4    0x20


// Read a 32-bit value from an I/O port
static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ __volatile__("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Write a 32-bit value to an I/O port
static inline void outl(uint16_t port, uint32_t value) {
    __asm__ __volatile__("outl %0, %1" : : "a"(value), "Nd"(port));
}

// Read a 32-bit value from PCI configuration space
uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp   = 0;

    // Create configuration space address
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                          (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write address to the address port
    outl(0xCF8, address);
    // Read data from the data port
    tmp = inl(0xCFC);

    return (tmp);
}

void detect_ide_controllers() {
    uint16_t bus, device, function; // Change data type to uint16_t
    uint16_t vendor_id, class_subclass; // Change data type to uint16_t
    uint8_t prog_if;
    
    // Scan the PCI bus
    for (bus = 0; bus < 256; bus++) { // Change data type to uint16_t
        for (device = 0; device < 32; device++) {
            for (function = 0; function < 8; function++) {
                // Read the Vendor ID register
                vendor_id = pci_config_read_dword(bus, device, function, PCI_CONFIG_VENDOR_ID) & 0xFFFF;
                
                // Check if the device is present
                if (vendor_id != 0xFFFF) {
                    // Read the Class Code and Subclass Code
                    class_subclass = pci_config_read_dword(bus, device, function, PCI_CONFIG_CLASS) >> 8;
                    // Check if it's a Mass Storage Controller and IDE subclass
                    if ((class_subclass == ((PCI_CLASS_MASS_STORAGE << 8) | PCI_SUBCLASS_IDE))) {
                        // Read the Programming Interface byte
                        prog_if = (pci_config_read_dword(bus, device, function, PCI_CONFIG_CLASS) >> 8) & 0xFF;

                        // Check for IDE programming interface bits
                        if (prog_if & IDE_PROG_IF_PRIMARY_NATIVE_MODE) {
                            // IDE controller is in PCI native mode for primary channel
                            // Read BARs and configure I/O ports
                        }
                        // Check other programming interface bits and handle accordingly
                    }
                }
            }
        }
    }
}



