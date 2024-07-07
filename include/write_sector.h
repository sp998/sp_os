#ifndef WRITE_SECTOR_H
#define WRITE_SECTOR_H

extern void write_sector(unsigned char drive, unsigned int sector_number, char *buffer);
extern unsigned char get_drive_number();
// Prototype for the assembly function
extern void write_to_disk(int disk_number, int lba, void* buffer, int num_sectors);



#endif /* WRITE_SECTOR_H */
