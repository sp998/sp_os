// gdt.c
#include <gdt.h>


extern void gdt_flush(void* gdt_ptr);

struct gdt_entry_struct gdt[5];
struct gdt_ptr_struct gdt_ptr;

void init_gdt() {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
    gdt_ptr.base = (uint32_t)&gdt;
    setGdtGate(0, 0, 0, 0, 0);                // Null segment
    setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    setGdtGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
    gdt_flush(&gdt_ptr); 
}

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].flags = (limit >> 16) & 0x0F;

    gdt[num].flags |= flags & 0xF0;
    gdt[num].access = access;
}
