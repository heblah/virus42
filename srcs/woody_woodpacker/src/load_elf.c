
#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/wait.h>

int get_n_files(void);
const unsigned char *get_hex_content(int i);
int get_hex_size(int i);

static char * add_str(char *str, int i, char *added)
{
	int size = 250 - i;

	while (size > 0 && *added != '\0')
	{
		str[i++] = *added++;
		size--;
	}
	return str;
}

static void add_itoa(char *dest, int *len, int n)
{
	char c = n % 10 + '0';

	if (n > 9)
		add_itoa(dest, len, n / 10);
	dest[*len] = c;
	*len += 1;
	return;
}

static int ft_strlen(char *s)
{
	int len = 0;

	if (s)
	{
		while (*s++)
			len++;
	}
	else 
		return -1;
	return len;
}

static int load_elf(void)
{
	uint8_t *elf = 0;
	int size = 0, len = 0;
	const int n_files = get_n_files();
	char woody[] = "...WOODY...\n";
	char path[250] = {0};
	char *arg[2] = {path, NULL};
	int fd, pid, ppid, i = 0;

	pid = syscall(SYS_getpid);
	while (i < n_files)
	{
		elf = (uint8_t *)get_hex_content(i);
		size = get_hex_size(i);
		for (int32_t byte = 0; byte < size; byte++)
		{
			/*
			 * XORing only certain bytes to decrypt
			 */
			if (byte % 2 == 0 || byte % 7 == 0 || byte % 11 == 0 || byte % 13 == 0)
				elf[byte] ^= 0xff;
		}
		fd = syscall(SYS_memfd_create, "elf", 0);
		if (fd == -1)
			return -1;
		if (write(fd, elf, size) != size)
			return -1;

		/*
		 * path to execute the fd from memfd_create:
		 *  - /proc/$pid/fd/$fd
		 */
		add_str(path, 0, "/proc/");
		len = ft_strlen(path);
		add_itoa(path, &len, pid);
		add_str(path, ft_strlen(path), "/fd/");
		len = ft_strlen(path);
		add_itoa(path, &len, fd);

		ppid = fork();
		if (ppid == 0)
		{
			write(1, woody, sizeof(woody) - 1);
			if (syscall(SYS_execve, path, arg, NULL))
				return -1;
		}
		else 
		{
			waitpid(ppid, 0, 0);
			for (int i = 0; path[i] != '\0'; i++)
				path[i] = 0;
			len = 0;
			close(fd);
		}
		i++;
	}
	return 0;
}

int main()
{
	if (load_elf() == -1)
		return -1;
	return 0;
}
