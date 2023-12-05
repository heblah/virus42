#include <sys/mman.h>
#include "headers.h"
// https://www.man7.org/linux/man-pages/man5/elf.5.html

    // static inline Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr) {
    // return (Elf64_Shdr *)((long)hdr + hdr->e_shoff);
    // }

    // static inline Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx) {
    // return &elf_sheader(hdr)[idx];
    // }

    // static inline char *elf_str_table(Elf64_Ehdr *hdr) {
    // if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
    // return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
    // }

    // static inline char *elf_lookup_string(Elf64_Ehdr *hdr, int offset) {
    // char *strtab = elf_str_table(hdr);
    // if(strtab == NULL) return NULL;
    // return strtab + offset;
    // }



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


// https://stackoverflow.com/questions/70583281/print-the-names-of-the-sections-headers-of-an-elf-file
    off_t text_offset = 0;
    Elf64_Shdr shdr;
    char* SectNames = NULL;
    // Elf64_Shdr shstrtab_shdr, shdr;
    // for (int i = 0; i < ehdr.e_shnum; ++i) {

    // first, read its header
    lseek(destination_fd, ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize, SEEK_SET);
    read(destination_fd, &shdr, sizeof(shdr));

    // next, read the section, string data
    SectNames = malloc(shdr.sh_size);
    lseek(destination_fd, shdr.sh_offset, SEEK_SET);
    read(destination_fd, SectNames, shdr.sh_size);


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

        char* section_name = "";
        section_name = SectNames + shdr.sh_name;
        // printf("%d\n", shdr.sh_name);
        // // printf("%s", section_name);
        if (shdr.sh_type == SHT_PROGBITS && strcmp(section_name, ".text") == 0)
        {
                
        //     section_name = elf_lookup_string(&ehdr, shdr.sh_name);
            printf("%s\n", section_name);
            text_offset = shdr.sh_offset;
            printf("%ld\n",  shdr.sh_offset);
        //     printf("offset = %d\n", shdr.sh_name);
        }
        // if (shdr.sh_type == SHT_PROGBITS && strcmp(section_name, ".text") == 0 ) {
        //     text_offset = shdr.sh_offset;
        //     }
    }
    free(SectNames);

    printf("%ld\n",  shdr.sh_offset);
    printf("%ld\n",  text_offset);
    printf("%ld\n",  shdr.sh_size);
    printf("%d\n",  destination_fd);
    
    // long page_size = sysconf(_SC_PAGESIZE);
    // if (text_offset % page_size != 0) {
    //     printf("text_offset is not a multiple of the page size\n");
    //     return 1;
    // }
    //Map the .text section into memory
    // void* text = malloc(shdr.sh_size);

    
    // Get the page size
    // long page_size = sysconf(_SC_PAGESIZE);
    // printf("%ld\n", page_size);
    // Check that text_offset is a multiple of the page size
    long page_size = 4096; // because size paging on a 64 bit system is 4096
    if (text_offset % page_size != 0) {
        text_offset = ((text_offset / page_size) + 1) * page_size;
    }

    void* text = mmap(NULL, shdr.sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, destination_fd, text_offset);
    if (text == MAP_FAILED) {
        perror("mmap");
        close(destination_fd);
        return 1;
    }

    // Change the protection of the memory region to PROT_WRITE
    if (mprotect(text, shdr.sh_size, PROT_WRITE) == -1) {
        perror("mprotect");
        return 1;
    }

    // Obfuscate the .text section
    uint8_t key = 0x55; // You can choose any key you like
    for (size_t i = 0; i < shdr.sh_size; ++i) {
        ((uint8_t*)text)[i] ^= key;
    }

    // // Change the protection of the memory region to PROT_EXEC
    // if (mprotect(text, shdr.sh_size, PROT_EXEC) == -1) {
    //     perror("mprotect");
    //     return 1;
    // }

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
    // free(text);
    return 0;
}