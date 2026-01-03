#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define PAGE_PRESENT 1
#define PAGE_RW      2
#define PAGE_USER    4

#ifdef __cplusplus
extern "C" {
#endif

void init_vmm();
void switch_page_directory(void* pd);
void map_page(void* phys, void* virt, uint32_t flags);
void enable_paging();

#ifdef __cplusplus
}
#endif

#endif
