#include <stddef.h> // For size_t

// Assume these are defined in your linker script
extern char _heap_start;
extern char _heap_end;

static char *heap_ptr = (char *)&_heap_start; // Pointer to track current position in heap

// Simple structure for memory blocks
typedef struct Block {
    size_t size;    // Size of the block
    struct Block *next; // Pointer to the next block
    int free;       // Free flag
} Block;

static Block *freeList = (Block *)&_heap_start; // Free list starts at the beginning of the heap

void init_malloc() {
    // Initialize the first block in the free list
    freeList->size = (size_t)(&_heap_end - &_heap_start) - sizeof(Block);
    freeList->next = NULL;
    freeList->free = 1; // Mark as free
}

void *malloc(size_t size) {
    Block *current = freeList;
    size += sizeof(Block); // Adjust size to account for the block metadata

    while (current) {
        if (current->free && current->size >= size) {
            current->free = 0; // Mark block as used
            if (current->size > size + sizeof(Block)) { // Split block if necessary
                Block *nextBlock = (Block *)((char *)current + size);
                nextBlock->size = current->size - size;
                nextBlock->next = current->next;
                nextBlock->free = 1;
                current->size = size;
                current->next = nextBlock;
            }
            return (char *)current + sizeof(Block); // Return pointer to usable memory
        }
        current = current->next;
    }
    return NULL; // No suitable block found
}

void free(void *ptr) {
    if (!ptr) return;
    Block *block = (Block *)((char *)ptr - sizeof(Block));
    block->free = 1; // Mark block as free
}
