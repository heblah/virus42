#include "headers.h"

// helpful sources:
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

    // Write the assembly code to the new section in the file
    lseek(destination_fd, new_shdr.sh_offset, SEEK_SET);
    write(destination_fd, asm_code, asm_code_size);

    // Update the section header table and the program header table to include the new section
    lseek(destination_fd, ehdr->e_shoff + ehdr->e_shnum * sizeof(Elf64_Shdr), SEEK_SET);
    write(destination_fd, &new_shdr, sizeof(new_shdr));

    lseek(destination_fd, ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf64_Phdr), SEEK_SET);
    write(destination_fd, last_plt_load_segment, sizeof(*last_plt_load_segment));

    // Update the section and program header counts in the ELF header
    ehdr->e_shnum++;
    ehdr->e_phnum++;

    lseek(destination_fd, 0, SEEK_SET);
    write(destination_fd, ehdr, sizeof(*ehdr));
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

    // Elf64_Shdr* last_section_in_plt_load_segment = find_last_section_in_plt_load_segment(shdr, ehdr, last_plt_load_segment);
    add_new_section(destination_fd, ehdr, last_plt_load_segment, asm_code, sizeof(asm_code), shdr);

    return 0;
}





//     Elf64_Shdr* strtab_shdr = &shdr[ehdr->e_shstrndx];
//     // Allocate memory for the .strtab section
//     char* strtab = malloc(strtab_shdr->sh_size);
//     if (!strtab) {
//         perror("malloc");
//         exit(EXIT_FAILURE);
//     }

//     // Move the file pointer to the start of the .strtab section
//     lseek(destination_fd, strtab_shdr->sh_offset, SEEK_SET);

//     // Read the .strtab section into the allocated memory
//     read(destination_fd, strtab, strtab_shdr->sh_size);

//     // Elf64_Shdr* strtab_shdr = &shdr[ehdr->e_shstrndx];
//     // char* strtab = mmap(NULL, strtab_shdr->sh_size, PROT_READ | PROT_WRITE, MAP_SHARED, destination_fd, strtab_shdr->sh_offset);
//     // if (strtab == MAP_FAILED) {
//     //     perror("Error mapping .strtab section");
//     //     // munmap(shdrs, ehdr->e_shnum * sizeof(Elf64_Shdr));
//     //     munmap(ehdr, sizeof(*ehdr));
//     //     close(destination_fd);
//     //     return 1;
//     // }

//     // ! 

//     const char* section_name = ".my_section";
//     size_t section_name_len = strlen(section_name);

//     // Append the section name to the .strtab section
//     char* new_strtab = malloc(strtab_shdr->sh_size + section_name_len + 1);
//     if (!new_strtab) {
//         perror("malloc");
//         exit(EXIT_FAILURE);
//     }

//     // Copy the existing .strtab section into the new memory area
//     memcpy(new_strtab, strtab, strtab_shdr->sh_size);

//     // Append the new section name to the new .strtab section
//     strcpy(new_strtab + strtab_shdr->sh_size, section_name);

//     // Write the new .strtab section back to the file
//     lseek(destination_fd, strtab_shdr->sh_offset, SEEK_SET);
//     write(destination_fd, new_strtab, strtab_shdr->sh_size + section_name_len + 1);

//     // // Unmap the old .strtab section and map the new one
//     // munmap(strtab, strtab_shdr->sh_size);
//     // strtab = mmap(NULL, strtab_shdr->sh_size + section_name_len + 1, PROT_READ | PROT_WRITE, MAP_SHARED, destination_fd, strtab_shdr->sh_offset);
//     // memcpy(strtab, new_strtab, strtab_shdr->sh_size + section_name_len + 1);
//     // free(new_strtab);

//     // Update the sh_size field of the .strtab section header to reflect the new size of the .strtab section
//     strtab_shdr->sh_size += section_name_len + 1;

//     // lseek(destination_fd, sh_offset, SEEK_SET);
//     // write(destination_fd, strtab_shdr, sizeof(Elf64_Shdr));

//     // ! CHECK INSERTION SECTION
//     // * first, read its header
//     // lseek(destination_fd, ehdr->e_shoff + ehdr->e_shstrndx * ehdr->e_shentsize, SEEK_SET);
//     // read(destination_fd, &shdr, sizeof(shdr));

//     // // * next, read the section, string data
//     // char* SectNames = malloc(shdr->sh_size); // malloc was used to circumvent issues with mmap but it could be used with a memory page check and mprotect
//     // lseek(destination_fd, shdr->sh_offset, SEEK_SET);
//     // read(destination_fd, SectNames, shdr->sh_size);





//     // check that the future section has been added to the index, and find its position
//     size_t strtab_index = 0;
//     // while (strtab_index < strtab_shdr->sh_size && strncmp(strtab + strtab_index, section_name, section_name_len) != 0) {
//     //     strtab_index++;
//     // }

//     // while (strtab_index <= strtab_shdr->sh_size - section_name_len && strncmp(strtab + strtab_index, section_name, section_name_len) != 0) {
//     //     strtab_index++;
//     // }

//     // if (strtab_index >= strtab_shdr->sh_size) {
//     //     printf("Section name not found in .strtab\n");
//     //     free(new_strtab);
//     //     // munmap(strtab, strtab_shdr->sh_size);
//     //     // munmap(shdrs, ehdr->e_shnum * sizeof(Elf64_Shdr));
//     //     // munmap(ehdr, sizeof(*ehdr));
//     //     close(destination_fd);
//     //     return 1;
//     // }









//     // * New section that will be inserted
//     Elf64_Shdr new_shdr;
//     ft_memset(&new_shdr, 0, sizeof(shdr));
//     new_shdr.sh_name = strtab_index; // Index of section name in .strtab
//     new_shdr.sh_type = SHT_PROGBITS; // Type of section
//     new_shdr.sh_flags = SHF_ALLOC | SHF_EXECINSTR; // Section flags
//     new_shdr.sh_addr = last_plt_load_segment->p_vaddr + last_plt_load_segment->p_filesz; // Address in memory
//     new_shdr.sh_offset = last_plt_load_segment->p_offset + last_plt_load_segment->p_filesz; // Offset in file
//     new_shdr.sh_size = sizeof(asm_code); // Size of section
//     new_shdr.sh_link = 0; // Link to another section
//     new_shdr.sh_info = 0; // Miscellaneous information
//     new_shdr.sh_addralign = 1; // Required alignment
//     new_shdr.sh_entsize = 0; // Size of each entry in the section


//     // Allocate memory for the new section
//     char* new_section = malloc(new_shdr.sh_size);
//     if (!new_section) {
//         perror("malloc");
//         exit(EXIT_FAILURE);
//     }

//     // Copy the assembly code into the new section
//     memcpy(new_section, asm_code, sizeof(asm_code));

//     // Move the file pointer to the start of the new section
//     lseek(destination_fd, new_shdr.sh_offset, SEEK_SET);

//     // Write the new section to the file
//     write(destination_fd, new_section, new_shdr.sh_size);

//     // Free the allocated memory for the new section
//     free(new_section);





//     // // Write the new section to the file
//     // char* new_section = mmap(NULL, new_shdr.sh_size, PROT_READ | PROT_WRITE, MAP_SHARED, destination_fd, new_shdr.sh_offset);
//     // if (new_section == MAP_FAILED) {
//     //     perror("Error mapping new section");
//     //     // munmap(shdr, sizeof(shdr));
//     //     // munmap(phdr, ehdr->e_phnum * sizeof(Elf64_Phdr));
//     //     munmap(ehdr, file_size + sizeof(Elf64_Phdr));
//     //     close(destination_fd);
//     //     return 1;
//     // }

//     // memcpy(new_section, asm_code, sizeof(asm_code));
//     // munmap(new_section, new_shdr.sh_size);

//     // Update the program header to reflect the new section
//     last_plt_load_segment->p_filesz += new_shdr.sh_size;
//     last_plt_load_segment->p_memsz += new_shdr.sh_size;


//     // Move the file position indicator to the new section's offset
//     lseek(destination_fd, new_shdr.sh_offset, SEEK_SET);

//     // Write the new section to the file
//     write(destination_fd, asm_code, sizeof(asm_code));

//     // Move the file position indicator to the new section's header's offset
//     lseek(destination_fd, ehdr->e_shoff + (ehdr->e_shnum - 1) * sizeof(Elf64_Shdr), SEEK_SET);

//     // // Write the new section header to the file
//     // write(destination_fd, &new_shdr, sizeof(new_shdr));

//     // Move the file position indicator to the program header's offset
//     lseek(destination_fd, ehdr->e_phoff + (ehdr->e_phnum - 1) * sizeof(Elf64_Phdr), SEEK_SET);

//     // Write the updated program header to the file
//     write(destination_fd, last_plt_load_segment, sizeof(*last_plt_load_segment));

//     // Increment the section and program header counts in the ELF header
//     ehdr->e_shnum++;
//     ehdr->e_phnum++;

//     // Move the file position indicator to the ELF header's offset
//     lseek(destination_fd, 0, SEEK_SET);

//     // Write the updated ELF header to the file
//     write(destination_fd, ehdr, sizeof(*ehdr));







// ! Open the ELF file and map it into memory:
// int fd = open("your_elf_file", O_RDWR);
// if (fd == -1) {
//     perror("Error opening file");
//     exit(EXIT_FAILURE);
// }

// struct stat sb;
// if (fstat(fd, &sb) == -1) {
//     perror("Error getting file size");
//     exit(EXIT_FAILURE);
// }

// void* map = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
// if (map == MAP_FAILED) {
//     perror("Error mapping file");
//     exit(EXIT_FAILURE);
// }

// // ! Get the ELF header and the program headers from the mapped memory:
// Elf64_Ehdr* ehdr = (Elf64_Ehdr*)map;
// Elf64_Phdr* phdr = (Elf64_Phdr*)(map + ehdr->e_phoff);


// ! Find the last PT_LOAD segment:
// Elf64_Phdr* last_plt_load_segment = NULL;
// for (int i = 0; i < ehdr->e_phnum; ++i) {
//     if (phdr[i].p_type == PT_LOAD && /* condition to identify PLT_LOAD */) {
//         last_plt_load_segment = &phdr[i];
//     }
// }

// ! Create a new segment containing the C function:
// Function to print "..WOODY.."
// void print_wood() {
//     printf("..WOODY..\n");
// }

// ! Create new program header
// Elf64_Phdr new_phdr = {
//     .p_type = PT_LOAD,
//     .p_flags = PF_X | PF_R,
//     .p_offset = last_plt_load_segment->p_offset + last_plt_load_segment->p_filesz,
//     .p_vaddr = last_plt_load_segment->p_vaddr + last_plt_load_segment->p_memsz,
//     .p_paddr = last_plt_load_segment->p_paddr + last_plt_load_segment->p_memsz,
//     .p_filesz = sizeof(print_wood),
//     .p_memsz = sizeof(print_wood),
//     .p_align = 16,
// };

// //  Copy the function to the new segment
// memcpy((char*)map + new_phdr.p_offset, print_wood, sizeof(print_wood));

// // ! Insert the new segment after the last PT_LOAD segment:
// // Shift existing segments
// size_t shift_size = sizeof(Elf64_Phdr) * ehdr->e_phnum;
// void* new_map = realloc(map, sb.st_size + sizeof(Elf64_Phdr));
// if (!new_map) {
//     perror("Error reallocating memory");
//     exit(EXIT_FAILURE);
// }

// memmove((char*)new_map + ehdr->e_phoff + sizeof(Elf64_Phdr), (char*)new_map + ehdr->e_phoff, shift_size);

// // Insert new segment
// memcpy((char*)new_map + ehdr->e_phoff + sizeof(Elf64_Phdr) * ehdr->e_phnum, &new_phdr, sizeof(Elf64_Phdr));

// // Update ELF header
// ehdr = (Elf64_Ehdr*)new_map;
// ehdr->e_phnum += 1;

// // ! Unmap the file from memory and close the file descriptor:
// munmap(new_map, sb.st_size + sizeof(Elf64_Phdr));
// close(fd);

