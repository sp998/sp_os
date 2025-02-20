#ifndef  MEM_H
#define MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

typedef struct MemBlock
{
    size_t size;
    struct MemBlock* next;
    int free;

}MemBlock;


void init_malloc();
void* malloc(size_t size);
void free(void* ptr);

#ifdef __cplusplus
}
#endif
#endif 