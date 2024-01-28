// ! Open the ELF file and map it into memory:
int fd = open("your_elf_file", O_RDWR);
if (fd == -1) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
}

struct stat sb;
if (fstat(fd, &sb) == -1) {
    perror("Error getting file size");
    exit(EXIT_FAILURE);
}

void* map = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (map == MAP_FAILED) {
    perror("Error mapping file");
    exit(EXIT_FAILURE);
}

// ! Get the ELF header and the program headers from the mapped memory:
Elf64_Ehdr* ehdr = (Elf64_Ehdr*)map;
Elf64_Phdr* phdr = (Elf64_Phdr*)(map + ehdr->e_phoff);


// ! Find the last PT_LOAD segment:
Elf64_Phdr* last_plt_load_segment = NULL;
for (int i = 0; i < ehdr->e_phnum; ++i) {
    if (phdr[i].p_type == PT_LOAD && /* condition to identify PLT_LOAD */) {
        last_plt_load_segment = &phdr[i];
    }
}

// ! Create a new segment containing the C function:
// Function to print "..WOODY.."
void print_wood() {
    printf("..WOODY..\n");
}

// ! Create new program header
Elf64_Phdr new_phdr = {
    .p_type = PT_LOAD,
    .p_flags = PF_X | PF_R,
    .p_offset = last_plt_load_segment->p_offset + last_plt_load_segment->p_filesz,
    .p_vaddr = last_plt_load_segment->p_vaddr + last_plt_load_segment->p_memsz,
    .p_paddr = last_plt_load_segment->p_paddr + last_plt_load_segment->p_memsz,
    .p_filesz = sizeof(print_wood),
    .p_memsz = sizeof(print_wood),
    .p_align = 16,
};

//  Copy the function to the new segment
memcpy((char*)map + new_phdr.p_offset, print_wood, sizeof(print_wood));

// ! Insert the new segment after the last PT_LOAD segment:
// Shift existing segments
size_t shift_size = sizeof(Elf64_Phdr) * ehdr->e_phnum;
void* new_map = realloc(map, sb.st_size + sizeof(Elf64_Phdr));
if (!new_map) {
    perror("Error reallocating memory");
    exit(EXIT_FAILURE);
}

memmove((char*)new_map + ehdr->e_phoff + sizeof(Elf64_Phdr), (char*)new_map + ehdr->e_phoff, shift_size);

// Insert new segment
memcpy((char*)new_map + ehdr->e_phoff + sizeof(Elf64_Phdr) * ehdr->e_phnum, &new_phdr, sizeof(Elf64_Phdr));

// Update ELF header
ehdr = (Elf64_Ehdr*)new_map;
ehdr->e_phnum += 1;

// ! Unmap the file from memory and close the file descriptor:
munmap(new_map, sb.st_size + sizeof(Elf64_Phdr));
close(fd);

