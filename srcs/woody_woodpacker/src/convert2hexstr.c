
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

static int	mk_namefile(const char *src, char *cfile)
{
	int	i = 0;

	while (src[i])
	{
		cfile[i] = src[i];
		i++;
	}	
	if (i + 6 <= NAME_MAX)
	{
		cfile[i++] = '_';
		cfile[i++] = 'h';
		cfile[i++] = 'e';
		cfile[i++] = 'x';
		cfile[i++] = '.';
		cfile[i++] = 'c';
	}
	return i + 6;
}

static void	src2hexstr(const unsigned char *data, int *size, int *cfile_fd, int col)
{
	if (col == 200)
		return;
	else if (col == 1)
		dprintf(*cfile_fd, "\t\t");
	dprintf(*cfile_fd, "0x%02x", *data);
	if (col % 12 == 0)
		dprintf(*cfile_fd, ",\n\t\t");
	else
		dprintf(*cfile_fd, ", ");
	//printf("bug at %d\n", col);
	src2hexstr(data + col + 1, size, cfile_fd, col + 1);
}

int	mk_cfile(const char * file)
{
	char			cfile_name[NAME_MAX] = {0};
	unsigned char	*buffer;
	int				cfile_fd = -1, src_fd = -1;
	int				size = 0;

	mk_namefile(file, cfile_name);
	cfile_fd = open(cfile_name, O_CREAT | O_RDWR, 0666);
	src_fd = open(file, O_RDONLY);
	if (cfile_fd == -1 || src_fd == -1)
	{
		perror("open");
		return -1;
	}
	size = lseek(src_fd, 0, SEEK_END);
	if (size == -1 || lseek(src_fd, 0, SEEK_SET) != 0)
	{
		perror("lseek");
		close(src_fd);
		close(cfile_fd);
		return -1;
	}
	buffer = malloc(size * sizeof(unsigned char));
	if (buffer == NULL)
	{
		perror("malloc");
		close(src_fd);
		close(cfile_fd);
		return -1;
	}
	printf("pid = %d\n", getpid());
	while(1);
	int rd = read(src_fd, buffer, 200);
	printf("read = %d\n", rd);
	if (rd == -1)
	{
		perror("read");
		close(src_fd);
		close(cfile_fd);
		free(buffer);
		return -1;
	}
	close(src_fd);
	src2hexstr(buffer, &size, &cfile_fd, 1);
	return 0;
}
