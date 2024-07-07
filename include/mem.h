#ifndef  MEM_H
#define MEM_H
#include <stddef.h>
#include <stdint.h>






struct mem_block {
     size_t size;
     struct mem_block* next;
};

void memcpy(void* src, const void* dest , size_t n);

void* malloc(size_t size);
void init_malloc();
void free(void*ptr);

#endif 