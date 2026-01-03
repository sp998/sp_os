#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <utils.h>
#include <printf.h>
#include <io.h>

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIR  1024

#define PD_INDEX(addr) (((uint32_t)(addr)) >> 22)
#define PT_INDEX(addr) ((((uint32_t)(addr)) >> 12) & 0x3FF)

static uint32_t* page_directory = 0;

void map_page(void* phys, void* virt, uint32_t flags) {
    uint32_t pdindex = PD_INDEX(virt);
    uint32_t ptindex = PT_INDEX(virt);

    uint32_t* pt = (uint32_t*)(page_directory[pdindex] & 0xFFFFF000);

    // If the page table is not present, allocate it
    if (!(page_directory[pdindex] & PAGE_PRESENT)) {
        pt = (uint32_t*)pmm_alloc_block();
        memset(pt, 0, 4096);
        
        // Mark directory entry as present and rw (and user if needed)
        // We generally allow RW for page tables
        page_directory[pdindex] = (uint32_t)pt | PAGE_PRESENT | PAGE_RW | PAGE_USER;
    }

    // Now map the page in the page table (Identity map for now or custom)
    // Note: since we don't have recursive paging setup or higher-half easy access yet,
    // we assume we can access 'pt' directly because we are mostly doing identity mapping
    // at initialization or working with physical addresses that are identity mapped.
    // WARNING: This logic breaks if 'pt' is not identity mapped. 
    // Since we only identity map for now, it holds.
    
    pt[ptindex] = ((uint32_t)phys) | (flags & 0xFFF) | PAGE_PRESENT;
    
    // Invalidate TLB (invlpg) would go here if we were changing an existing mapping
}

void enable_paging() {
    uint32_t cr0;
    
    asm volatile("mov %0, %%cr3":: "r"(page_directory));
    
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging bit
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void init_vmm() {
    print("Initializing VMM...\n");

    // 1. Allocate Page Directory
    page_directory = (uint32_t*)pmm_alloc_block();
    memset(page_directory, 0, 4096);

    // 2. Identity Map the first 4MB (Kernel + BIOS + Video)
    // We map using a single page table (4MB covers 0x0 to 0x400000)
    // Actually, let's just use map_page to loop
    // But map_page allocates a table too. Let's act implicit.
    
    // Start by identity mapping the full 0-4MB range where our kernel lives
    // (Assuming kernel < 4MB, which it definitely is)
    // map_page internally allocates tables.
    
    // Map 0x00000000 -> 0x00400000 (4MB)
    uint32_t i = 0;
    while (i < 0x400000) { // 4MB
        map_page((void*)i, (void*)i, PAGE_PRESENT | PAGE_RW | PAGE_USER);
        i += 4096; // Next page
    }
    
    // Also Identity map the PMM bitmap area if it ended up outside 4MB?
    // PMM size is 128MB.
    // If we want to access all physical RAM as a flat heap later, we should probably 
    // identity map *all* detected RAM for now (simplest "flat" paging mode).
    // Or just map what we need. 
    
    // Let's map 32MB for now, to be safe for heap growth and user programs (loaded at 0x1000000)
     while (i < 0x2000000) { // 32MB
        map_page((void*)i, (void*)i, PAGE_PRESENT | PAGE_RW | PAGE_USER);
        i += 4096;
    }

    // Register ISR 14 handler? (Later)

    printf("Enable Paging...\n");
    enable_paging();
    printf("Paging Enabled!\n");
}
