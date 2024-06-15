#ifndef ATA_H
#define ATA_H

#include <stdint.h>

extern void ata_lba_write(uint32_t lba, uint8_t sector_count, const char *buffer);
void detect_ide_controllers();

#endif  /* ATA_H */
