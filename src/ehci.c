#include "ehci.h"
#include "io.h" // Include your IO functions
#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static inline void outl(uint32_t port, uint32_t value) {
    asm volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint32_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
}

uint32_t pci_config_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return (1 << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC);
}

uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = pci_config_address(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

static void print_ehci_info(uint8_t bus, uint8_t device, uint8_t function, uint32_t bar) {
    print("EHCI Controller found at bus ");
    print_number(bus);
    print(", device ");
    print_number(device);
    print(", function ");
    print_number(function);
    print("\nEHCI MMIO Base Address: ");
    print_hex(bar);
    print("\n");
}

void pci_scan_for_ehci() {

    int count=0;
    for (size_t bus = 0; bus <254; bus++) { // Limit to 16 buses
   
    

        for (uint8_t device = 0; device < 32; device++) {
         
        
            for (uint8_t function = 0; function < 8; function++) {
               

                uint32_t vendor_id = pci_read_config_dword(bus, device, function, 0x00) & 0xFFFF;
                if (vendor_id == 0xFFFF) {
                    continue; // Skip if device not present
                    print("Device vendor not found");
                    print("\n");
                    update_display();
                    
                }

                uint32_t class_code = pci_read_config_dword(bus, device, function, 0x08);
                uint8_t base_class = (class_code >> 24) & 0xFF;
                uint8_t sub_class = (class_code >> 16) & 0xFF;
                uint8_t prog_if = (class_code >> 8) & 0xFF;
             
             
        

                if (base_class == 0x0C && sub_class == 0x03 && prog_if == 0x20) {
                    print_ehci_info(bus, device, function, pci_read_config_dword(bus, device, function, 0x10) & ~0xF);
                    update_display();
                   // Exit after finding the first EHCI controller
                }
            }
        }
    }
    print("EHCI Controller not found.\n");
}