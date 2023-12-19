
#if DEBUG

#include <stdio.h>
#include <elf.h>
#include "famine.h"

static void	print_ehdr(Elf64_Ehdr *ehdr)
{
	printf(	"----------------------------------------\n"
			"|                 ELF header\n"
			"| ehdr->e_ident    : %s\n"
			"| ehdr->e_type     : %u\n"
			"| ehdr->e_machine  : %u\n"
			"| ehdr->e_verion   : %u\n"
			"| ehdr->e_entry    : %p\n"
			"| ehdr->e_phoff    : %lu bytes\n"
			"| ehdr->e_shoff    : %lu bytes\n"
			"| ehdr->flags      : %u\n"
			"| ehdr->e_ehsize   : %u bytes\n"
			"| ehdr->e_phentsize: %u bytes\n"
			"| ehdr->e_phnum    : %u\n"
			"| ehdr->e_shentsize: %u bytes\n"
			"| ehdr->e_shnum    : %u\n"
			"| ehdr->e_shstrndx : %u\n",
				ehdr->e_ident,
				ehdr->e_type,
				ehdr->e_machine,
				ehdr->e_version,
				(void *)ehdr->e_entry,
				(unsigned long)ehdr->e_phoff,
				(unsigned long)ehdr->e_shoff,
				ehdr->e_flags,
				ehdr->e_ehsize,
				ehdr->e_phentsize,
				ehdr->e_phnum,
				ehdr->e_shentsize,
				ehdr->e_shnum,
				ehdr->e_shstrndx
			);
	return;
}

static void	print_phdr(Elf64_Phdr *phdr, int i)
{
	printf(	"----------------------------------------\n"
			"|                 Program header[%d]:%p\n"
			"| phdr->p_type  : %u\n"
			"| phdr->p_offset: %p\n"
			"| phdr->p_vaddr : %p\n"
			"| phdr->p_paddr : %p\n"
			"| phdr->p_filesz: 0x%lx\n"
			"| phdr->p_memsz : 0x%lx\n"
			"| phdr->p_flags : %u\n"
			"| phdr->p_align : %lu\n",
				i,
				(void *)phdr,
				phdr->p_type,
				(void *)phdr->p_offset,
				(void *)phdr->p_vaddr,
				(void *)phdr->p_paddr,
				phdr->p_filesz,
				phdr->p_memsz,
				phdr->p_flags,
				phdr->p_align
			);
	return;
}

static void print_shdr(Elf64_Shdr *shdr, int i)
{
	printf(	"----------------------------------------\n"
			"|                 Section header[%d]:%p\n"
			"| shdr->sh_name     : %u\n"
			"| shdr->sh_type     : %u\n"
			"| shdr->sh_flags    : %lu\n"
			"| shdr->sh_addr     : %p\n"
			"| shdr->sh_offset   : 0x%lx\n"
			"| shdr->sh_size     : %lu bytes\n"
			"| shdr->sh_link     : %u\n"
			"| shdr->sh_info     : %u\n"
			"| shdr->sh_addralign: %lu\n"
			"| shdr->sh_entsize  : %lu\n",
				i,
				(void *)shdr,
				shdr->sh_name,
				shdr->sh_type,
				shdr->sh_flags,
				(void *)shdr->sh_addr,
				shdr->sh_offset,
				shdr->sh_size,
				shdr->sh_link,
				shdr->sh_info,
				shdr->sh_addralign,
				shdr->sh_entsize
			);
	return;
}

void	print_headers(t_elf *elf)
{
	Elf64_Phdr	*phdr = elf->phdr;
	Elf64_Shdr	*shdr = elf->shdr;

	print_ehdr(elf->ehdr);
	for (int i = 0; i < elf->ehdr->e_phnum; i++)
		print_phdr(phdr + i, i);
	for (int i = 1; i < elf->ehdr->e_shnum; i++)
		print_shdr(shdr + i, i);
	return;
}

#endif
