#ifndef HEADERS_H
#define HEADERS_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <elf.h>
#include <string.h>
#include <stddef.h>

int copy_binary(int source_file);
off_t getFileSize(int fd);
int	ft_memcmp(const void *s1, const void *s2, size_t n);
void* ft_memset(void* ptr, int value, size_t num);
int obfuscate(void);
int injection();

#endif

// elf struct memo

// ehdr => elf header
//  typedef struct {
//     unsigned char e_ident[EI_NIDENT];
//     uint16_t      e_type;
//     uint16_t      e_machine;
//     uint32_t      e_version;
//     ElfN_Addr     e_entry;
//     ElfN_Off      e_phoff; =  File offset of the Program Header Table.
//     ElfN_Off      e_shoff; = File offset of the Section Header Table.
//     uint32_t      e_flags;
//     uint16_t      e_ehsize;
//     uint16_t      e_phentsize;
//     uint16_t      e_phnum; = Number of Program Headers.
//     uint16_t      e_shentsize;
//     uint16_t      e_shnum; = Number of Section Headers.
//     uint16_t      e_shstrndx; // section header table index
// } ElfN_Ehdr;

// phdr[] => program header table is an array of structures, each describing a segment or other
//         information the system needs to prepare the program for execution

// typedef struct {
//            uint32_t   p_type;
//            uint32_t   p_flags;
//            Elf64_Off  p_offset;
//            Elf64_Addr p_vaddr; virtual address at which the first byte of the segment resides in memory
//            Elf64_Addr p_paddr;
//            uint64_t   p_filesz;
//            uint64_t   p_memsz; number of bytes in the memory image of the segment
//            uint64_t   p_align;
//        } Elf64_Phdr;

// shdr[] => section header table

// typedef struct {
//        uint32_t   sh_name;
//        uint32_t   sh_type;
//        uint64_t   sh_flags;
//        Elf64_Addr sh_addr;
//        Elf64_Off  sh_offset;
//        uint64_t   sh_size;
//        uint32_t   sh_link;
//        uint32_t   sh_info;
//        uint64_t   sh_addralign;
//        uint64_t   sh_entsize;
//    } Elf64_Shdr;