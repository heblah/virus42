
/*
 * Project      : Famine
 * Author       : halvarez / plgueugnon
 * Starting date: 01/12/2023
 */

#include <stdint.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/memfd.h>
#include <fcntl.h>

#include "famine.h"

void	ft_fexecve(t_elf *target)
{
	char	*argv[1];

	argv[0] = NULL;
//	argv[1] = NULL;
	int shm_fd = shm_open("ft_elf", O_RDWR | O_CREAT, 0700);
	if (shm_fd == -1)
	{
		perror("shm_open");
		exit(EXIT_FAILURE);
	}
	if (write(shm_fd, target->ehdr, target->fsize) == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	while(1);
	// argv can't be nullable ?
	if (fexecve(shm_fd, argv, NULL) == -1)
	{
		perror("fexecve");
		exit(EXIT_FAILURE);
	}
	return;
}

void imexecve(t_elf *target)
{
	long	mem_fd = syscall(SYS_memfd_create, "in_mem", MFD_CLOEXEC);
	char	buf[100] = {0};
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

void	mkelf2cfile(const char *path2file)
{
	int	elf = open(path2file, O_RDONLY, 0666);
	int c_file = open("elf_data.c", O_RDWR | O_CREAT | O_TRUNC, 0666);
	ssize_t fsize = lseek(elf, 0, SEEK_END);
	char *start = "static uint8_t data_elf ='";
	void *buffer = fsize != -1 ? malloc(fsize * sizeof(char)) : NULL;
	char *end = "'";

	if (buffer != NULL && lseek(elf, 0, SEEK_SET) != -1)
	{
		if (write(c_file, start, strlen(start)) == -1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("malloc || lseek");
		exit(EXIT_FAILURE);
	}
	if (read(elf, buffer, fsize) == -1)
	{
		perror("read");
		exit(EXIT_FAILURE);
	}
	if (write(c_file, buffer, fsize) == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	if (write(c_file, end, strlen(end)) == -1)
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	close(elf);
	close(c_file);
	return;
}

int	main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	t_elf	target;
	t_elf	payload __attribute__((unused));
	int		pid = getpid();

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
	printf("pid = %d\n", pid);
	target.fd = elf_open_and_map(&target);
	mkelf2cfile(target.fname);
	//imexecve(&target);
	//ft_fexecve(&target);
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
