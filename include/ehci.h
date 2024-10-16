#ifndef EHCI_H
#define EHCI_H

#include <stdint.h>
#include<io.h>

// Function to scan for EHCI controllers on PCI
void pci_scan_for_ehci();

#endif // EHCI_H
