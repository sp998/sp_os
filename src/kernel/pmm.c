#include <kernel/pmm.h>
#include <utils.h>
#include <printf.h>
#include <sysvars.h>
#include <io.h>

// 32 blocks per byte (oops, 8 blocks per byte, 32 blocks per uint32_t)
#define BLOCKS_PER_BYTE 8

// Size of physical memory to manage (e.g., 128MB for now, can be dynamic)
#define PMM_MEM_SIZE 0x8000000 
#define PMM_BITMAP_SIZE (PMM_MEM_SIZE / PMM_BLOCK_SIZE / BLOCKS_PER_BYTE)

static uint8_t pmm_bitmap[PMM_BITMAP_SIZE];
static uint32_t used_blocks = 0;
static uint32_t max_blocks = PMM_MEM_SIZE / PMM_BLOCK_SIZE;

// Internal helper to set a bit in the bitmap
static void mmp_set(uint32_t bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
}

// Internal helper to unset a bit in the bitmap
static void mmp_unset(uint32_t bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

// Internal helper to test if a bit is set
static int mmp_test(uint32_t bit) {
    return pmm_bitmap[bit / 8] & (1 << (bit % 8));
}

// Find the first free block
static int mmp_first_free() {
    for (uint32_t i = 0; i < max_blocks / 32; i++) {
        uint32_t* ptr = (uint32_t*)&pmm_bitmap[i * 4];
        if (*ptr != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                int bit = 1 << j;
                if (!(*ptr & bit)) {
                    return i * 32 + j;
                }
            }
        }
    }
    return -1;
}

void pmm_init_region(uint32_t start, size_t size) {
    uint32_t align = start / PMM_BLOCK_SIZE;
    uint32_t blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--) {
        if (align >= max_blocks) break; // Bounds check
        mmp_unset(align++);
        used_blocks--;
    }
    
    mmp_set(0); // Always ensure NULL (0x0) is marked used to avoid null pointer confusion
}

void pmm_deinit_region(uint32_t start, size_t size) {
    uint32_t align = start / PMM_BLOCK_SIZE;
    uint32_t blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--) {
        mmp_set(align++);
        used_blocks++;
    }
}

void init_pmm(multiboot_info_t* mboot_info) {
    // 1. Mark everything as used initially (safe default)
    memset(pmm_bitmap, 0xFF, PMM_BITMAP_SIZE);
    used_blocks = max_blocks;

    // 2. Parse Multiboot (GRUB) memory map to find legitimate free RAM
    // Note: We need to be careful not to overwrite the kernel itself!
    // But for simplicity in this step, we will assume the map tells us what is RAM.
    
    print("Initializing PMM...\n");

    if (mboot_info->flags & MULTIBOOT_INFO_MEM_MAP) {
        multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mboot_info->mmap_addr;
        while ((uint32_t)mmap < mboot_info->mmap_addr + mboot_info->mmap_length) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                // Initialize this region as free
                // We assume start_addr is 32-bit for now (i686)
                // Use .addr_low because of 64-bit split in multiboot struct
                pmm_init_region((uint32_t)mmap->addr_low, (uint32_t)mmap->len_low);
            }
            mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
        }
    } else {
        print("PMM: No memory map provided by bootloader!\n");
    }

    // 3. Mark Kernel area as USED so we don't allocate over it
    // We assume kernel is loaded at 1MB (0x100000) and link.ld defines _end
    extern uint32_t _end;
    uint32_t kernel_end = (uint32_t)&_end;
    // Mark from 0x0 to kernel_end as used
    // (0x0 to 0x100000 is BIOS/GRUB stuff, 0x100000 to _end is Kernel)
    pmm_deinit_region(0x0, kernel_end + 4096); // +4k safety margin

    printf("PMM Initialized. Used blocks: %d / %d\n", (int)used_blocks, (int)max_blocks);
}

void* pmm_alloc_block() {
    if (max_blocks - used_blocks <= 0) return NULL; // Out of memory

    int frame = mmp_first_free();
    if (frame == -1) return NULL;

    mmp_set(frame);
    used_blocks++;
    
    uint32_t addr = frame * PMM_BLOCK_SIZE;
    return (void*)addr;
}

void pmm_free_block(void* p) {
    uint32_t addr = (uint32_t)p;
    int frame = addr / PMM_BLOCK_SIZE;
    
    mmp_unset(frame);
    used_blocks--;
}
