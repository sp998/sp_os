#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <multiboot.h>
#include <stddef.h>

#define PMM_BLOCK_SIZE 4096

#ifdef __cplusplus
extern "C" {
#endif

void init_pmm(multiboot_info_t* mboot_info);
void* pmm_alloc_block();
void pmm_free_block(void* p);
void pmm_init_region(uint32_t start, size_t size);
void pmm_deinit_region(uint32_t start, size_t size);

#ifdef __cplusplus
}
#endif

#endif
