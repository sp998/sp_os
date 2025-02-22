#include <ide.h>

// Function to read a word from an I/O port
uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %0, %1" : : "a" (data), "Nd" (port));
}

// Function to initialize IDE controller
bool init_ide() {
    // Send a reset command to the IDE controller
    outb(IDE_CONTROL, IDE_CONTROL_SRST_BIT);
    
    // Wait a short period
    for (int i = 0; i < 100000; i++) {
        asm volatile ("nop");
    }
    
    // Wait for the controller to clear the reset bit
    for (int i = 0; i < 1000; i++) {
        uint8_t status = inb(IDE_STATUS);
        if (!(status & IDE_STATUS_BSY_BIT)) {
            return true;
        }
    }

    return false;
}


// Function to write data to disk using IDE controller
// Function to write data to disk using IDE controller
bool ide_write_disk(uint32_t sector, void* data, uint32_t sector_count) {
    if (!init_ide()) {
  
        return false;
    }

    // Select disk (master) and sector(s) to write to
    outb(IDE_SECTOR_COUNT, sector_count & 0xFF);
    outb(IDE_SECTOR_NUMBER, sector & 0xFF);
    outb(IDE_CYLINDER_LOW, (sector >> 8) & 0xFF);
    outb(IDE_CYLINDER_HIGH, (sector >> 16) & 0xFF);
    outb(IDE_HEAD, 0xE0 | ((sector >> 24) & 0x0F)); // Master drive, LBA mode

    // Send write command (0x30) to IDE controller
    outb(IDE_COMMAND, 0x30);

    // Wait for the controller to signal data request (DRQ) status
    bool drq_set = false;
    for (int i = 0; i < 100000; i++) {
        uint8_t status = inb(IDE_STATUS);
        if (status & IDE_STATUS_DRQ_BIT) {
            drq_set = true;
            break;
        }
        asm volatile ("nop");
    }

    // Check if DRQ bit is set, otherwise return false
    if (!drq_set) {
       
        return false;
    }

    // Write data to disk
    uint16_t* buffer = (uint16_t*)data;
    for (uint32_t i = 0; i < sector_count * 256; ++i) {
        outw(IDE_DATA, buffer[i]);
    }

    // Wait for the controller to clear the busy bit
    for (int i = 0; i < 100000; i++) {
        if (!(inb(IDE_STATUS) & IDE_STATUS_BSY_BIT)) {
            return true;
        }
        asm volatile ("nop");
    }

    return false;
}


bool clear_disk(uint32_t sector,uint32_t sector_count){

     if (!init_ide()) {
    
        return false;
    }

    // Select disk (master) and sector(s) to write to
    outb(IDE_SECTOR_COUNT, sector_count & 0xFF);
    outb(IDE_SECTOR_NUMBER, sector & 0xFF);
    outb(IDE_CYLINDER_LOW, (sector >> 8) & 0xFF);
    outb(IDE_CYLINDER_HIGH, (sector >> 16) & 0xFF);
    outb(IDE_HEAD, 0xE0 | ((sector >> 24) & 0x0F)); // Master drive, LBA mode

    // Send write command (0x30) to IDE controller
    outb(IDE_COMMAND, 0x30);

    // Wait for the controller to signal data request (DRQ) status
    bool drq_set = false;
    for (int i = 0; i < 100000; i++) {
        uint8_t status = inb(IDE_STATUS);
        if (status & IDE_STATUS_DRQ_BIT) {
            drq_set = true;
            break;
        }
        asm volatile ("nop");
    }

    // Check if DRQ bit is set, otherwise return false
    if (!drq_set) {
       
        return false;
    }

   
    for (uint32_t i = 0; i < sector_count * 256; ++i) {
        outw(IDE_DATA,0);
    }

    // Wait for the controller to clear the busy bit
    for (int i = 0; i < 100000; i++) {
        if (!(inb(IDE_STATUS) & IDE_STATUS_BSY_BIT)) {
            return true;
        }
        asm volatile ("nop");
    }

    return false;
}


// Function to read data from disk using IDE controller
bool ide_read_disk(uint32_t sector, void* data, uint32_t sector_count) {
    if (!init_ide()) {
       
        return false;
    }

    // Select disk (master) and sector(s) to read from
    outb(IDE_SECTOR_COUNT, sector_count & 0xFF);
    outb(IDE_SECTOR_NUMBER, sector & 0xFF);
    outb(IDE_CYLINDER_LOW, (sector >> 8) & 0xFF);
    outb(IDE_CYLINDER_HIGH, (sector >> 16) & 0xFF);
    outb(IDE_HEAD, 0xE0 | ((sector >> 24) & 0x0F)); // Master drive, LBA mode

    // Send read command (0x20) to IDE controller
    outb(IDE_COMMAND, 0x20);

    // Wait for the controller to signal data request (DRQ) status
    bool drq_set = false;
    for (int i = 0; i < 100000; i++) {
        uint8_t status = inb(IDE_STATUS);
        if (status & IDE_STATUS_DRQ_BIT) {
            drq_set = true;
            break;
        }
        asm volatile ("nop");
    }

    // Check if DRQ bit is set, otherwise return false
    if (!drq_set) {
      
        return false;
    }

    // Read data from disk
    uint16_t* buffer = (uint16_t*)data;
    for (uint32_t i = 0; i < sector_count * 256; ++i) {
        buffer[i] = inw(IDE_DATA);
    }

    // Wait for the controller to clear the busy bit
    for (int i = 0; i < 100000; i++) {
        if (!(inb(IDE_STATUS) & IDE_STATUS_BSY_BIT)) {
            return true;
        }
        asm volatile ("nop");
    }

    return false;
}


bool ide_write_int(uint32_t sector, uint32_t value) {
    uint16_t buffer[256] = {0}; // Create a buffer initialized to 0
    buffer[0] = value & 0xFFFF; // Lower 16 bits
    buffer[1] = (value >> 16) & 0xFFFF; // Upper 16 bits
    return ide_write_disk(sector, buffer, 1);
}

// Function to read an integer from disk
bool ide_read_int(uint32_t sector, uint32_t* value) {
    uint16_t buffer[256] = {0}; // Create a buffer
    if (ide_read_disk(sector, buffer, 1)) {
        *value = buffer[0] | (buffer[1] << 16);
        return true;
    }
    return false;
}




