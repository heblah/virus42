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
        printf("address of phdr %d\n", phdr[i].p_type);
        if (phdr[i].p_type == PT_LOAD) {
            last_plt_load_segment = &phdr[i];
        }
    }
    return last_plt_load_segment;
}

int injection() 
{
    int destination_fd = open("woody", O_RDWR);
    if (destination_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    ssize_t file_size = getFileSize(destination_fd);

    Elf64_Ehdr* ehdr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, destination_fd, 0);
    if (ehdr == MAP_FAILED) {
        perror("Error mapping file");
        exit(EXIT_FAILURE);
    }

    // Get the ELF header and the program headers from the mapped memory:
    // Elf64_Ehdr* ehdr = (Elf64_Ehdr*)map;
    Elf64_Phdr* phdr = (Elf64_Phdr*)(ehdr->e_phoff);
    Elf64_Shdr* shdr = (Elf64_Shdr*)(ehdr->e_shoff);

    Elf64_Phdr* last_plt_load_segment = find_last_plt_load_segment(phdr, ehdr);




    Elf64_Shdr* strtab_shdr = &shdr[ehdr->e_shstrndx];
    char* strtab = mmap(NULL, strtab_shdr->sh_size, PROT_READ | PROT_WRITE, MAP_SHARED, destination_fd, strtab_shdr->sh_offset);
    if (strtab == MAP_FAILED) {
        perror("Error mapping .strtab section");
        // munmap(shdrs, ehdr->e_shnum * sizeof(Elf64_Shdr));
        munmap(ehdr, sizeof(*ehdr));
        close(destination_fd);
        return 1;
    }

    const char* section_name = ".my_section";
    size_t section_name_len = strlen(section_name);

    // Append the section name to the .strtab section
    char* new_strtab = malloc(strtab_shdr->sh_size + section_name_len + 1);
    memcpy(new_strtab, strtab, strtab_shdr->sh_size);
    strcpy(new_strtab + strtab_shdr->sh_size, section_name);

    // Unmap the old .strtab section and map the new one
    munmap(strtab, strtab_shdr->sh_size);
    strtab = mmap(NULL, strtab_shdr->sh_size + section_name_len + 1, PROT_READ | PROT_WRITE, MAP_SHARED, destination_fd, strtab_shdr->sh_offset);
    memcpy(strtab, new_strtab, strtab_shdr->sh_size + section_name_len + 1);
    free(new_strtab);

    // Update the sh_size field of the .strtab section header to reflect the new size of the .strtab section
    strtab_shdr->sh_size += section_name_len + 1;

    // check that the future section has been added to the index, and find its position
    size_t strtab_index = 0;
    while (strtab_index < strtab_shdr->sh_size && strncmp(strtab + strtab_index, section_name, section_name_len) != 0) {
        strtab_index++;
    }

    if (strtab_index >= strtab_shdr->sh_size) {
        printf("Section name not found in .strtab\n");
        munmap(strtab, strtab_shdr->sh_size);
        // munmap(shdrs, ehdr->e_shnum * sizeof(Elf64_Shdr));
        munmap(ehdr, sizeof(*ehdr));
        close(destination_fd);
        return 1;
    }



    // * New section that will be inserted
    Elf64_Shdr new_shdr;
    ft_memset(&new_shdr, 0, sizeof(shdr));
    new_shdr.sh_name = strtab_index; // Index of section name in .strtab
    new_shdr.sh_type = SHT_PROGBITS; // Type of section
    new_shdr.sh_flags = SHF_ALLOC | SHF_EXECINSTR; // Section flags
    new_shdr.sh_addr = last_plt_load_segment->p_vaddr + last_plt_load_segment->p_filesz; // Address in memory
    new_shdr.sh_offset = last_plt_load_segment->p_offset + last_plt_load_segment->p_filesz; // Offset in file
    new_shdr.sh_size = sizeof(asm_code); // Size of section
    new_shdr.sh_link = 0; // Link to another section
    new_shdr.sh_info = 0; // Miscellaneous information
    new_shdr.sh_addralign = 1; // Required alignment
    new_shdr.sh_entsize = 0; // Size of each entry in the section

    // Write the new section to the file
    char* new_section = mmap(NULL, new_shdr.sh_size, PROT_READ | PROT_WRITE, MAP_SHARED, destination_fd, new_shdr.sh_offset);
    if (new_section == MAP_FAILED) {
        perror("Error mapping new section");
        // munmap(shdr, sizeof(shdr));
        // munmap(phdr, ehdr->e_phnum * sizeof(Elf64_Phdr));
        munmap(ehdr, file_size + sizeof(Elf64_Phdr));
        close(destination_fd);
        return 1;
    }

    memcpy(new_section, asm_code, sizeof(asm_code));
    munmap(new_section, new_shdr.sh_size);

    // Update the program header to reflect the new section
    last_plt_load_segment->p_filesz += new_shdr.sh_size;
    last_plt_load_segment->p_memsz += new_shdr.sh_size;


    // Move the file position indicator to the new section's offset
    lseek(destination_fd, new_shdr.sh_offset, SEEK_SET);

    // Write the new section to the file
    write(destination_fd, asm_code, sizeof(asm_code));

    // Move the file position indicator to the new section's header's offset
    lseek(destination_fd, ehdr->e_shoff + (ehdr->e_shnum - 1) * sizeof(Elf64_Shdr), SEEK_SET);

    // Write the new section header to the file
    write(destination_fd, &new_shdr, sizeof(new_shdr));

    // Move the file position indicator to the program header's offset
    lseek(destination_fd, ehdr->e_phoff + (ehdr->e_phnum - 1) * sizeof(Elf64_Phdr), SEEK_SET);

    // Write the updated program header to the file
    write(destination_fd, last_plt_load_segment, sizeof(*last_plt_load_segment));

    // Increment the section and program header counts in the ELF header
    ehdr->e_shnum++;
    ehdr->e_phnum++;

    // Move the file position indicator to the ELF header's offset
    lseek(destination_fd, 0, SEEK_SET);

    // Write the updated ELF header to the file
    write(destination_fd, ehdr, sizeof(*ehdr));

    return 0;
}





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

