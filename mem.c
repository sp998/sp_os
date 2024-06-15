#include "mem.h"
#define PAZE_SIZE 4096 
#define NUM_PAGES 100

static struct mem_block* freelist = NULL;
void init_malloc(void* mem_start,void* mem_end){
    freelist=(struct mem_block*)mem_start;
    freelist->size= (size_t)((char*)mem_end- (char*)mem_start);
    freelist->next=NULL;
}




 void memcpy(void* src, const void* dest , size_t n){
    // Cast the void* pointers to unsigned char* pointers for byte-wise copying
    unsigned char* d = (unsigned char*) dest;
    const unsigned char* s = (const unsigned char*) src;
    while (n--) {
        *d++ = *s++;
    }
    
}
void* malloc(size_t size){
   struct mem_block* curr = freelist;
   struct mem_block* prev = NULL;
   while (curr !=NULL && curr->size<size)
   {
     prev=curr;
     curr=curr->next;
   }
   if(curr==NULL){
    return NULL;
   }
   if(curr->size == size){
    if(prev==NULL){
        freelist=curr->next;
    }else{
        prev->next = curr->next;
    }
   }else{
    struct mem_block* new_block =(struct mem_block*)((char*)curr+size);
    new_block->size=curr->size-size;
    new_block->next= curr->next;
    if(prev==NULL){
        freelist=new_block;
    }else{
        prev->next=new_block;
    }
   }
   return curr;
   
}
void free(void*ptr){
    struct mem_block* block = (struct mem_block*)ptr;
    block->next=freelist;
    freelist=block;
}