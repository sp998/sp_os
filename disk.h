// disk.h
#ifndef DISK_H
#define DISK_H

void disk_init();
void disk_write_sector(unsigned int sector, void *data);
void ide_controller_init();
#endif
