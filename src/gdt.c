// gdt.c
#include <gdt.h>

//Extern the function from the assembly to load the GDT

extern void gdt_flush();

//Initialize the GDT

#define VGA_ADDRESS 0xB8000
#define WHITEO_ON_BLACK 0x0F


void write_string_to_vga(const char* str){
    unsigned short *vga=(unsigned short*)VGA_ADDRESS;
    while(*str){
        *vga++=(*str++ |WHITEO_ON_BLACK<<8);
    }
}

void init_gdt() {
    gdt_flush(); // Call assembly function to load  GDT
}

