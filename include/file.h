#ifndef FILE_H
#define FILE_H
#include <stdint.h>

typedef struct FileEntry{
 char name[10];
 char ext[3];
 uint16_t startLocation;
 struct FileEntry *next;
} File;
#endif