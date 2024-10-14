#ifndef  MEM_H
#define MEM_H
#include <stddef.h>
#include <stdint.h>
#define HEAP_SIZE (4*1024*1024) // Size of the heap (4 KB + 1 KB)

typedef struct Block {
    size_t size; // Size of the block
    struct Block *next; // Pointer to the next block
    int free; // Free flag
} Block;


void* malloc(size_t size);
void init_malloc();
void free(void*ptr);

#endif 