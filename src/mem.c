#include<mem.h>

extern char _heap_start;
extern char _heap_end;

MemBlock* freeList = (MemBlock*)&_heap_start;

void init_malloc(){
    freeList->size = (size_t)(&_heap_end-&_heap_start)-sizeof(MemBlock);
    freeList->next = NULL;
    freeList->free = TRUE;
}

void* malloc(size_t size){
    MemBlock* current =freeList;
    size+=sizeof(MemBlock);
    while (current)
    {
        if(current->free && current->size>=size){
            current->free=FALSE;

            if(current->size>size+sizeof(MemBlock)){
                MemBlock* nextBlock =(MemBlock*)((char*)current+size);
                nextBlock->size = current->size-size;
                nextBlock->free = TRUE;
                nextBlock->next=current->next;
                current->size=size;
                current->next=nextBlock;
            }

            return (char*)current+sizeof(MemBlock);
        }
    
    current=current->next;
    }
    return NULL;
}

void free(void* ptr){
    if(!ptr) return;
    MemBlock* block =(MemBlock*)((char*)ptr-sizeof(MemBlock));
    block->free=TRUE;
}