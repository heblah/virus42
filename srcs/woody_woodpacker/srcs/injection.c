#include "headers.h"

// helpful sources:
// https://0x00sec.org/t/elfun-file-injector/410
// https://github.com/0x050f/woody-woodpacker/tree/main
// https://github.com/sebastiencs/Packer_ELF/blob/master/src/insert_section.c
// https://github.com/SilentVoid13/Silent_Packer/tree/master/src/ELF

const char asm_code[] = "\xb8\x04\x00\x00\x00\xbb\x04\x00\x00\x00\xb9\x01\x00\x00\x00\xba\x00\x00\x00\x00\xcd\x80\xe8\xe5\xff\xff\xff\x48\x65\x6c\x6c\x6f\x2c\x20\x57\x4f\x4f\x44\x59\x21\x0a";

Elf64_Phdr* find_last_plt_load_segment(Elf64_Phdr* phdr, Elf64_Ehdr* ehdr)
{
    Elf64_Phdr* last_plt_load_segment = NULL;
    for (int i = 0; i < ehdr->e_phnum; ++i) {
        printf("index %i\n", i);
        printf("type set at phdr %d\n", phdr[i].p_type);
        if (phdr[i].p_type == PT_LOAD) {
            last_plt_load_segment = &phdr[i];
            printf("index of PLT_LOAD= %i & type = %i\n", i, phdr[i].p_type);
        }
    }
    return last_plt_load_segment;
}

// Elf64_Shdr* find_last_section_in_plt_load_segment(Elf64_Shdr* shdr, Elf64_Ehdr* ehdr, Elf64_Phdr* plt_load_segment) {
//     Elf64_Shdr* last_section = NULL;
//     for (int i = 0; i < ehdr->e_shnum; ++i) {
//         if (shdr[i].sh_addr >= plt_load_segment->p_vaddr && 
//             shdr[i].sh_addr + shdr[i].sh_size <= plt_load_segment->p_vaddr + plt_load_segment->p_memsz) {
//             last_section = &shdr[i];
//         }
//     }
//     return last_section;
// }

void fix_sh_link(Elf64_Shdr* shdr, Elf64_Ehdr* ehdr) {
    for (int i = 0; i < ehdr->e_shnum -1; ++i) {
        if (shdr[i].sh_link >= ehdr->e_shnum) {
            printf("Invalid sh_link value in section header %d\n", i);
            shdr[i].sh_link = 0;
        }
    }
}



    // ! Pour nommer et identifier notre section, on doit modifier la table des section et y ajouter l'index de notre section
void append_section_name(int destination_fd, Elf64_Ehdr* ehdr, Elf64_Shdr* shdr, Elf64_Shdr* new_shdr, const char* section_name) {
    Elf64_Shdr* strtab_shdr = &shdr[ehdr->e_shstrndx];

    // Allocate memory for the new .strtab section
    char* new_strtab = malloc(strtab_shdr->sh_size + strlen(section_name) + 1);
    if (!new_strtab) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Copy the existing .strtab section into the new memory area
    lseek(destination_fd, strtab_shdr->sh_offset, SEEK_SET);
    read(destination_fd, new_strtab, strtab_shdr->sh_size);

    // Append the new section name to the new .strtab section
    strcpy(new_strtab + strtab_shdr->sh_size, section_name);

    // Write the new .strtab section back to the file
    lseek(destination_fd, strtab_shdr->sh_offset, SEEK_SET);
    write(destination_fd, new_strtab, strtab_shdr->sh_size + strlen(section_name) + 1);

    // Update the size of the .strtab section header
    strtab_shdr->sh_size += strlen(section_name) + 1;

    // Write the updated section header back to the file
    lseek(destination_fd, ehdr->e_shoff + ehdr->e_shstrndx * sizeof(Elf64_Shdr), SEEK_SET);
    write(destination_fd, strtab_shdr, sizeof(Elf64_Shdr));

    // Find the position of the new section name in the .strtab section
    size_t strtab_index = 0;
    while (strtab_index < strtab_shdr->sh_size && strncmp(new_strtab + strtab_index, section_name, strlen(section_name)) != 0) {
        strtab_index++;
    }

    if (strtab_index >= strtab_shdr->sh_size) {
        printf("Section name not found in .strtab\n");
        free(new_strtab);
        close(destination_fd);
        return ;
    }

    // Update the sh_name field of the new section's header
    new_shdr->sh_name = strtab_index;

    // Free the allocated memory for the new .strtab section
    free(new_strtab);
}

// ! on ajoute une fonction à la fin du dernier segment PLT_LOAD
void add_new_section(int destination_fd, Elf64_Ehdr* ehdr, Elf64_Phdr* last_plt_load_segment, const char* asm_code, size_t asm_code_size,  Elf64_Shdr* shdr) {
    // Create a new section header for the new section
    Elf64_Shdr new_shdr;
    memset(&new_shdr, 0, sizeof(new_shdr));
    new_shdr.sh_name = 0; /* Index of the section name in the .strtab section */
    new_shdr.sh_type = SHT_PROGBITS; // Type of the new section
    new_shdr.sh_flags = SHF_ALLOC | SHF_EXECINSTR; // Flags of the new section
    new_shdr.sh_addr = last_plt_load_segment->p_vaddr + last_plt_load_segment->p_filesz; // Address of the new section in memory
    new_shdr.sh_offset = last_plt_load_segment->p_offset + last_plt_load_segment->p_filesz; // Offset of the new section in the file
    new_shdr.sh_size = asm_code_size; // Size of the new section
    new_shdr.sh_link = 0; // Link to another section
    new_shdr.sh_info = 0; // Miscellaneous information
    new_shdr.sh_addralign = 1; // Required alignment
    new_shdr.sh_entsize = 0; // Size of each entry in the section

    append_section_name(destination_fd, ehdr, shdr, &new_shdr, ".my_section");


    // // Calculate the index of the symbol table section
    // int symtab_index = -1;
    // for (int i = 0; i < ehdr->e_shnum; ++i) {
    //     if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM) {
    //         symtab_index = i;
    //         break;
    //     }
    // }

    // if (symtab_index != -1) {
    //     // Set the sh_info field to the index of the symbol table section
    //     new_shdr.sh_info = symtab_index;
    // } else {
    //     printf("No symbol table section found\n");
    // }


    // Write the assembly code to the new section in the file
    lseek(destination_fd, new_shdr.sh_offset, SEEK_SET);
    write(destination_fd, asm_code, asm_code_size);


    // // Calculate the offset of the original entry point relative to the end of the new section
    // uint32_t offset = ehdr->e_entry - (new_shdr.sh_addr + new_shdr.sh_size);

    // // Convert the offset into a jump instruction
    // char jmp_instruction[5] = {0xe9};
    // memcpy(jmp_instruction + 1, &offset, sizeof(offset));

    // // Write the jump instruction to the new section
    // lseek(destination_fd, new_shdr.sh_offset + new_shdr.sh_size, SEEK_SET);
    // write(destination_fd, jmp_instruction, sizeof(jmp_instruction));

    // // Update the size of the new section to include the jump instruction
    // new_shdr.sh_size += sizeof(jmp_instruction);


    // Update the section header table and the program header table to include the new section
    lseek(destination_fd, ehdr->e_shoff + ehdr->e_shnum * sizeof(Elf64_Shdr), SEEK_SET);
    write(destination_fd, &new_shdr, sizeof(new_shdr));

    lseek(destination_fd, ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf64_Phdr), SEEK_SET);
    write(destination_fd, last_plt_load_segment, sizeof(*last_plt_load_segment));

    // Update the section and program header counts in the ELF header
    ehdr->e_shnum++;
    // ehdr->e_phnum++;

    // Update the entry point of the ELF header to point to the new section
    ehdr->e_entry = new_shdr.sh_addr;

    // Increment the sh_link field of all subsequent section headers
    for (int i = ehdr->e_shnum; i < ehdr->e_shnum - 1; ++i) {
        shdr[i].sh_link++;
    }

    // Update the e_shstrndx field of the ELF header
    ehdr->e_shstrndx++;

    lseek(destination_fd, 0, SEEK_SET);
    write(destination_fd, ehdr, sizeof(*ehdr));

    // fix_sh_link(shdr, ehdr);

    
}


int injection() 
{
    int destination_fd = open("woody", O_RDWR);
    if (destination_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    ssize_t file_size = getFileSize(destination_fd);

    // Allocate memory for the ELF header
    Elf64_Ehdr* ehdr = malloc(file_size);
    if (!ehdr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Read the ELF header from the file
    lseek(destination_fd, 0, SEEK_SET);
    read(destination_fd, ehdr, file_size);

    

    // Get the section header and the program headers from the mapped memory:
    Elf64_Phdr* phdr = malloc(ehdr->e_phnum * sizeof(Elf64_Phdr));
    if (!phdr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    lseek(destination_fd, ehdr->e_phoff, SEEK_SET);
    read(destination_fd, phdr, ehdr->e_phnum * sizeof(Elf64_Phdr));


    Elf64_Shdr* shdr = malloc(ehdr->e_shnum * sizeof(Elf64_Shdr));
    if (!phdr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    lseek(destination_fd, ehdr->e_shoff, SEEK_SET);
    read(destination_fd, shdr, ehdr->e_shnum * sizeof(Elf64_Shdr));

    Elf64_Phdr* last_plt_load_segment = find_last_plt_load_segment(phdr, ehdr);

    // ! So far so good =
    // ! On map les header + section + on trouve le dernier segment.

    add_new_section(destination_fd, ehdr, last_plt_load_segment, asm_code, sizeof(asm_code), shdr);

    return 0;
}