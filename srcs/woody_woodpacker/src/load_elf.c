
#include<stdio.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/wait.h>
#include<sys/mman.h>

int get_n_files(void);
const unsigned char *get_hex_content(int i);
int get_hex_size(int i);

static int load_elf(void)
{
	unsigned char const *elf = 0;
	int size = 0;
	const int n_files = get_n_files();
	char path[250] = {0};
	char *arg[2] = {path, NULL};
	int fd, pid, child, i = 0;

	pid = syscall(SYS_getpid);
	while (i < n_files)
	{
		elf = get_hex_content(i);
		size = get_hex_size(i);
		fd = syscall(SYS_memfd_create, "elf", 0);
		if (fd == -1)
			return -1;
		if (write(fd, elf, size) != size)
			return -1;
		sprintf(path, "/proc/%d/fd/%d", pid, fd);
		child = fork();
		if (child == 0)
		{
			if (execve(path, arg, NULL) == -1)
			{
				printf("Error execve\n");
				return -1;
			}
		}
		else 
		{
			waitpid(child, 0, 0);
			close(fd);
		}
		i++;
	}
	return 0;
}

int main()
{
	load_elf();
	return 0;
}
