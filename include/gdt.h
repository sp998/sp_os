#ifndef GDT_H
#define GDT_H

void init_gdt();
void write_string_to_vga(const char* str);
extern void test_memory_access();

#endif