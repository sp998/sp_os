// gdt.c
#include <gdt.h>
#include <utils.h>


extern void gdt_flush(void* gdt_ptr);
extern void tss_flush();

struct gdt_entry_struct gdt[6];
struct gdt_ptr_struct gdt_ptr;
struct tss_entry_struct tss_entry;

void init_gdt() {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt;
    setGdtGate(0, 0, 0, 0, 0);                // Null segment
    setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    setGdtGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
    writeTSS(5,0x10,0x0); // TSS
    gdt_flush(&gdt_ptr); 
    tss_flush();
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

void writeTSS(uint32_t num, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);
    setGdtGate(num, base, limit, 0xE9, 0x00);
    memset(&tss_entry, 0, sizeof(tss_entry));
    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x08 | 0x03;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x03;
    
}
