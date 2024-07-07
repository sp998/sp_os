#ifndef IDE_H
#define IDE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>



// Define IDE controller registers
#define IDE_DATA             0x1F0
#define IDE_ERROR            0x1F1
#define IDE_SECTOR_COUNT     0x1F2
#define IDE_SECTOR_NUMBER    0x1F3
#define IDE_CYLINDER_LOW     0x1F4
#define IDE_CYLINDER_HIGH    0x1F5
#define IDE_HEAD             0x1F6
#define IDE_COMMAND          0x1F7
#define IDE_STATUS           0x1F7
#define IDE_CONTROL          0x3F6

// Define IDE controller register bits
#define IDE_CONTROL_SRST_BIT (1 << 2)
#define IDE_STATUS_BSY_BIT   (1 << 7)
#define IDE_STATUS_DRQ_BIT   (1 << 3)

// Function to initialize IDE controller
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t data);
bool ide_write_int(uint32_t sector, uint32_t value);
bool ide_read_int(uint32_t sector, uint32_t* value);
bool init_ide();
bool ide_write_disk(uint32_t sector, void* data, uint32_t sector_count);
bool ide_read_disk(uint32_t sector, void* data, uint32_t sector_count);
bool clear_disk(uint32_t sector,uint32_t sector_count);


#endif