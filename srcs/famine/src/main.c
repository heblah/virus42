
/*
 * Project      : Famine
 * Author       : halvarez / plgueugnon
 * Starting date: 01/12/2023
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/memfd.h>

#include "famine.h"


void imexecve(t_elf *target)
{
	long	mem_fd = syscall(SYS_memfd_create, "in_mem", MFD_CLOEXEC);
	char	buf[100];
	char	ls[100];
	int		pid = getpid();
	char	*file[2];

	file[0] = buf;
	file[1] = NULL;
	if(write(mem_fd, target->ehdr, target->fsize) == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	printf("pid = %d\n", pid);
	sprintf(buf, "/proc/%d/fd/%ld", pid, mem_fd);
	sprintf(ls, "ls -l /proc/%d/fd/", pid);
	system(ls);
	if (execve(file[0], file, NULL) == 1)
	{
		close(mem_fd);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	return;
}

int	main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	t_elf	target;
	t_elf	payload;

	/*
	if (argc != 3)
	{
#if DEBUG
		fprintf(stderr, "Usage: %s elf_file payload\n", argv[0]);
#endif
		return 1;
	}
	*/
	target.fname = argv[1];
	payload.fname = argv[2];

	/* Processing target */
	target.fd = elf_open_and_map(&target);
	imexecve(&target);
	//elf_x_section_counter(target.shdr, target.ehdr->e_shnum);
	//print_headers(&target);
	//elf_find_gap(&target);

	/*
#if DEBUG
	printf("+ Target entrypoint: %p\n", (void *)target_entrypoint);
#endif
	target_text_segment = elf_find_gap(target_data, target_fsize, &p, &len);

	// Payload part
	payload_fd = elf_open_and_map(argv[2], &payload_data, &payload_fsize);
	return 0;
	*/
}
