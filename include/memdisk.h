#ifndef MEMDISK_H
#define MEMDISK_H
#include <utils.h>
#include <string.h>
#define RAM_DISK_SIZE  1024*1024

void init_mem_disk();

void wirte_to_mem_disk(int offset,const char* data,int length);

void read_from_mem_disk(int offset,char* buffer, int length);

#endif

