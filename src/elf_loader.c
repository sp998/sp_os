#include<elf_loader.h>
#include<string.h>
#include<fat32.h>


bool check_entry_point(Elf32_Ehdr* ehdr,  uint8_t* elf_buffer,uint32_t entry_point);

bool load_elf_from_disk(uint32_t start_sector, void* buffer) {
    uint32_t offset = 0;  // Current offset in the buffer

    // Read full sectors
    for (uint32_t i = 0; i < ELF_SECTOR_COUNT; i++) {
        if (!ide_read_disk(start_sector + i, (uint8_t*)buffer + offset, 1)) {
            printf("Error reading sector %u\n", start_sector + i);
            return false;  // Failed to read a sector
        }
        offset += SECTOR_SIZE;
    }

    return true;  // Successfully loaded the ELF file
}



uint32_t load_and_print_elf(void* elf_buffer) {
    // Parse ELF header
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)elf_buffer;
    

    // Verify ELF magic
    if (ehdr->e_ident[0] != 0x7f || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
        printf("Invalid ELF magic\n");
        return;
    }
    

    if(check_entry_point(ehdr,elf_buffer,ehdr->e_entry)==0){
        return NULL;
    }
    // Loop through program headers
    Elf32_Phdr* phdrs = (Elf32_Phdr*)(elf_buffer + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf32_Phdr* ph = &phdrs[i];

        if (ph->p_type != 1) continue; // Only load PT_LOAD

        //printf("Loading segment %d to virtual address 0x%x\n", i, ph->p_vaddr);

        // Copy segment data from file to memory
        memcpy((void*)ph->p_vaddr, elf_buffer + ph->p_offset, ph->p_filesz);

        // Zero the remaining memory if p_memsz > p_filesz
        if (ph->p_memsz > ph->p_filesz) {
            memset((void*)(ph->p_vaddr + ph->p_filesz), 0, ph->p_memsz - ph->p_filesz);
        }
    }

   // printf("ELF Loaded. Entry Point: 0x%x\n", ehdr->e_entry);
    return ehdr->e_entry;
}

uint32_t load_elf_directly(uint32_t start_sector, void *buffer)
{
    if(load_elf_from_disk(start_sector,buffer)){
        return load_and_print_elf(buffer);
    }
    return NULL;
}

bool check_entry_point(Elf32_Ehdr* ehdr,  uint8_t* elf_buffer,uint32_t entry_point){
    Elf32_Shdr* sh_table = (Elf32_Shdr*)(elf_buffer + ehdr->e_shoff);
    Elf32_Shdr* symtab_hdr = NULL;
    Elf32_Shdr* strtab_hdr = NULL;


    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (sh_table[i].sh_type == SHT_SYMTAB) symtab_hdr = &sh_table[i];
        if (sh_table[i].sh_type == SHT_STRTAB && i != ehdr->e_shstrndx) strtab_hdr = &sh_table[i];
    }
    
    if (symtab_hdr && strtab_hdr) {
        Elf32_Sym* symbols = (Elf32_Sym*)(elf_buffer + symtab_hdr->sh_offset);
        const char* strtab = (const char*)(elf_buffer + strtab_hdr->sh_offset);
        int symbol_count = symtab_hdr->sh_size / sizeof(Elf32_Sym);
    
        for (int i = 0; i < symbol_count; i++) {
            if (symbols[i].st_value == entry_point) {
                //printf("entry point : %s",&strtab[symbols[i].st_name]);
               if(strcmp(&strtab[symbols[i].st_name],"sp_main")==0){
        
                return true;
               }else{
                return false;
               }
            }
        }
    }
    return false;
}