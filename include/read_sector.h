#ifndef DISK_IO_H
#define DISK_IO_H

#ifdef __cplusplus
extern "C" {
#endif

char* read_sector(unsigned char drive, unsigned int sector_number);

#ifdef __cplusplus
}
#endif

#endif /* DISK_IO_H */
