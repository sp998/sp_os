ENTRY(loader)

SECTIONS {
    /* Ensure the Multiboot header is the first thing in the binary */
    . = 0x00100000; /* the code should be loaded at 1 MB */

    .multiboot_header ALIGN(4) :
    {
        KEEP(*(.multiboot_header))
    }

    .text ALIGN(0x1000) : /*align at 4 KB */
    {
        *(.text)
    }

.bss ALIGN(4) :
{
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
    _end = .;
}
}
