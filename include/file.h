#ifndef FILE_H
#define FILE_H
#include <stdint.h>
#include <stdbool.h>

typedef struct FileEntry{
 char* name;
 char* ext;
 uint32_t offset;
 uint32_t size;
 struct FileEntry *next;
 bool is_directory;
 bool is_root;
 uint32_t level;
 struct  FileEntry *parent;
} File;


File* create_file(char* name);
File* create_directory(char* name);
void add_file(File* rootFile,char* name);
void add_directory(File* rootFile,char* name);

void list_files(File* rootFile);
void init_root();
File* get_root();
File* find_file(File* rootFile,char* name);

void set_current_directory(File* new_root);
File* get_current_directory();

#endif