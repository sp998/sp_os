#include <io.h>
#include <achi.h>
bool ahci_controller_init() {
    // Check if AHCI controller is already enabled
    if (*(volatile uint32_t*)AHCI_GLOBAL_CONTROL & AHCI_GLOBAL_CONTROL_AHCI_ENABLE_BIT) {
        print("AHCI controller is already enabled.\n");
        return true;  // AHCI controller is already initialized
    }

    // Enable AHCI controller
    *(volatile uint32_t*)AHCI_GLOBAL_CONTROL |= AHCI_GLOBAL_CONTROL_AHCI_ENABLE_BIT;

    // Wait for AHCI controller to become ready (you may need to implement additional checks)
    // Example: wait for AHCI controller to clear the init bit in the capabilities register
    while (*(volatile uint32_t*)AHCI_CAPABILITIES & (1 << 31)) {
        // Wait for the controller to become ready
    }

    // Perform additional AHCI controller initialization steps here
    // Example: configure command lists, command tables, etc.

    print("AHCI controller initialized.\n");

    return true;
}