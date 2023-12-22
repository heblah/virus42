#include "headers.h"
// * https://www.man7.org/linux/man-pages/man5/elf.5.html

int obfuscate(void)
{
    int destination_fd = open("woody", O_RDWR);
    if (destination_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // * Parse the ELF header to find the offset of the .text section
    Elf64_Ehdr ehdr;
    if (read(destination_fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        perror("read 1");
        close(destination_fd);
        return 1;
    }

    /*
    * Seek from the start of the ELF header and read it into a buffer. 
    * The offset of the .text section is stored in the sh_offset field of the section header 
    * that has its sh_name field set to ".text".
    * Useful source: https://stackoverflow.com/questions/70583281/print-the-names-of-the-sections-headers-of-an-elf-file
    **/ 

    off_t text_offset = 0;
    Elf64_Shdr shdr;
    char* SectNames = NULL;

    // * first, read its header
    lseek(destination_fd, ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize, SEEK_SET);
    read(destination_fd, &shdr, sizeof(shdr));

    // * next, read the section, string data
    SectNames = malloc(shdr.sh_size); // malloc was used to circumvent issues with mmap but it could be used with a memory page check and mprotect
    lseek(destination_fd, shdr.sh_offset, SEEK_SET);
    read(destination_fd, SectNames, shdr.sh_size);

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

        // * Each loop section_name takes the address of the section string address name
        char* section_name = "";
        section_name = SectNames + shdr.sh_name;

        // * Each loop we ensure the section we are looking at contains executable information and the string ".text"
        if (shdr.sh_type == SHT_PROGBITS && strcmp(section_name, ".text") == 0)
        {
            text_offset = shdr.sh_offset;
        }
    }
    free(SectNames);

    // * Check that text_offset is a multiple of the page size
    long page_size = 4096; // because size paging on a 64 bit system is 4096 and we can't use sysconf
    if (text_offset % page_size != 0) {
        text_offset = ((text_offset / page_size) + 1) * page_size;
    }

    // * Map the .text section into memory
    void* text = mmap(NULL, shdr.sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, destination_fd, text_offset);
    if (text == MAP_FAILED) {
        perror("mmap");
        close(destination_fd);
        return 1;
    }

    // * Change the protection of the memory region to PROT_WRITE
    if (mprotect(text, shdr.sh_size, PROT_WRITE) == -1) {
        perror("mprotect");
        return 1;
    }

    // * Obfuscate the .text section
    // TODO Use an encryption algorithm
    uint8_t key = 0x55; // You can choose any key you like
    for (size_t i = 0; i < shdr.sh_size; ++i) {
        ((uint8_t*)text)[i] ^= key;
    }

    // * Write the obfuscated .text section back to the binary file
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