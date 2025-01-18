#ifndef FILE_H
#define FILE_H
#include <stdint.h>

typedef struct FileEntry{
 char* name;
 char* ext;
 uint16_t startLocation;
 struct FileEntry *next;
} File;


File* create_file(char* name);
void add_file(File* rootFile,char* name);

void list_files(File* rootFile);
void init_root();
File* get_root();
#endif