
#include "headers.h"
// https://www.man7.org/linux/man-pages/man5/elf.5.html

static inline Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr) {
	return (Elf64_Shdr *)((long)hdr + hdr->e_shoff);
}

    static inline Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx) {
        return &elf_sheader(hdr)[idx];
    }

    // static inline char *elf_str_table(Elf64_Ehdr *hdr) {
    //     if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
    //     return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
    // }
 
static inline char *elf_str_table(Elf64_Ehdr *hdr) {
   if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
   off_t offset = ((hdr->e_shstrndx)*hdr->e_shentsize)+hdr->e_shoff;
   return (char *)hdr + offset;
}

    static inline char *elf_lookup_string(Elf64_Ehdr *hdr, int offset) {
            char *strtab = elf_str_table(hdr);
            if(strtab == NULL) return NULL;
            return strtab + offset;
    }

int obfuscate(void)
{
    int destination_fd = open("woody", O_RDWR);
    if (destination_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Parse the ELF header to find the offset of the .text section
    Elf64_Ehdr ehdr;
    if (read(destination_fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        perror("read 1");
        close(destination_fd);
        return 1;
    }

    // Seek from the start of the ELF header and read it into a buffer. 
    // The offset of the .text section is stored in the sh_offset field of the section header 
    // that has its sh_name field set to ".text".



    off_t text_offset = 0;
     Elf64_Shdr shdr;
    // Elf64_Shdr shstrtab_shdr, shdr;
    // for (int i = 0; i < ehdr.e_shnum; ++i) {


        // if (ehdr.e_shstrndx[i].sh_type == SHT_STRTAB)
        // // if (lseek(destination_fd, ehdr.e_shoff + i * ehdr.e_shentsize, SEEK_SET) == -1) {
        // //     perror("lseek");
        // //     exit(EXIT_FAILURE);
        // // }
        // // if (read(destination_fd, &shdr, sizeof(shdr)) != sizeof(shdr)) {
        // //     perror("read");
        // //     exit(EXIT_FAILURE);
        // // }
        // if (shdr.sh_type == SHT_STRTAB) {
        //     shstrtab_shdr = ehdr.e_shstrndx[i];
        //     }
    // }

    // if (shstrtab_shdr.sh_type != SHT_STRTAB) {
    //     fprintf(stderr, "Could not find .shstrtab section\n");
    //     exit(EXIT_FAILURE);
    // }

    // printf("\n%ld\n",shstrtab_shdr.sh_size);
    // printf("%ld\n",shstrtab_shdr.sh_offset);
    // printf("%d\n",destination_fd);

    // char* shstrtab = ehdr + shstrtab_shdr.sh_offset;
    // char* shstrtab = mmap(NULL, shstrtab_shdr.sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, destination_fd, shstrtab_shdr.sh_offset);
    // if (shstrtab == MAP_FAILED) {
    //     perror("mmap 1");
    //     exit(EXIT_FAILURE);
    // }

    for (int i = 0; i < ehdr.e_shnum; ++i) {
        if (lseek(destination_fd, ehdr.e_shoff + i * ehdr.e_shentsize, SEEK_SET) == -1) {
            perror("lseek");
            close(destination_fd);
            return 1;
        }

        if (read(destination_fd, &shdr, sizeof(shdr)) != sizeof(shdr)) {
            perror("read 2");
            close(destination_fd);
            return 1;
        }

        // char* section_name = elf_lookup_string(&ehdr, shdr.sh_name);
        printf("%d\n", shdr.sh_name);
        // printf("%s", section_name);
        // if (shdr.sh_type == SHT_PROGBITS && strcmp(section_name, ".text") == 0 ) {
        //     text_offset = shdr.sh_offset;
        //     }
    }

    //Map the .text section into memory
    void* text = mmap(NULL, shdr.sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, destination_fd, text_offset);
    if (text == MAP_FAILED) {
        perror("mmap");
        close(destination_fd);
        return 1;
    }

    // Obfuscate the .text section
    uint8_t key = 0x55; // You can choose any key you like
    for (size_t i = 0; i < shdr.sh_size; ++i) {
        ((uint8_t*)text)[i] ^= key;
    }

    //Write the obfuscated .text section back to the binary file
    if (lseek(destination_fd, text_offset, SEEK_SET) == -1) {
        perror("lseek");
        close(destination_fd);
        return 1;
    }

    if ((long unsigned int)write(destination_fd, text, shdr.sh_size) != shdr.sh_size) {
        perror("write");
        close(destination_fd);
        return 1;
    }

    return 0;
}