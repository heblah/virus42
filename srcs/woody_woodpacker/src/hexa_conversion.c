
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

static void	itoa(int fd, int n)
{
	char c = n % 10 + '0';

	if (n > 9)
		itoa(fd, n / 10);
	write(fd, &c, 1);
}

/* Functions to write the hexadecimal representation of the packed-files */

static void init_content_file(int content_fd, int n_files)
{
	char define[] = "\n#define N_FILES ";
	char func[] = "\n\nconst unsigned char *get_hex_content(int i)\n{\n";

	write(content_fd, define, sizeof(define) - 1);
	itoa(content_fd, n_files);
	write(content_fd, func, sizeof(func) - 1);
	return;
}

/*
static int	write_hex_content(int content_fd, int src_fd)
{
	return 0;
}
*/

static void close_content_file(int content_fd, int n_files)
{
	int i = 0;
	char definition[] = "\tstatic unsigned char *files[N_FILES];\n";
	char files[] = "\tfiles[", file[] = "] = file", eol[] = ";\n";
	char eofunc[] = "\treturn i < N_FILES ? files[i] : 0;\n}\n";

	write(content_fd, definition, sizeof(definition) - 1);
	while(i < n_files)
	{
		write(content_fd, files, sizeof(files) - 1);
		itoa(content_fd, i);
		write(content_fd, file, sizeof(file) - 1);
		itoa(content_fd, i);
		write(content_fd, eol, sizeof(eol) - 1);
		i++;
	}
	write(content_fd, eofunc, sizeof(eofunc) -1);
}

static int	hexa_file(char **files, int n_files, int *size)
{
	int i = 0, src_fd = -1;
	int content_fd = open("hex_content_file.c", O_CREAT | O_RDWR, 0666);

	if (content_fd == -1)
		return -1;
	init_content_file(content_fd, n_files);
	while (i < n_files)
	{
		src_fd = open(files[i], O_RDONLY);
		if (src_fd == -1)
			return -1;
		size[i] = lseek(src_fd, 0, SEEK_END);
		if (size[i] == -1 || lseek(src_fd, 0, SEEK_SET) != 0)
		`	return -1;
		//write_hex_content();
		close(src_fd);
		i++;
	}
	close_content_file(content_fd, n_files);
	close(content_fd);
	return 0;
}
/* End of functions to write the hexadecimal representation of the packed-files */


/* Functions to write the sizes of the packed-files */

static void write_size_file(int size_fd, int n_files, int *size)
{
	char define[] = "\n#define N_FILES ";
	char func[] = "int get_hex_size(int i)\n{\n\tstatic int *sizes[N_FILES];\n";
	char size_def[] = "\tstatic int size";
	char size_addr[] = "] = &size";
	char sizes[] = "sizes[";
	char equal[] = " = ";
	char eol[] = ";\n";
	char eofunc[] = "\treturn i < N_FILES ? sizes[i] ? -1;\n}\n";
	int i = 0;

	write(size_fd, define, sizeof(define) - 1);
	itoa(size_fd, n_files);
	write(size_fd, func, sizeof(func) - 1);
	while(i < n_files)
	{
		/* declaration of sizeX */
		write(size_fd, size_def, sizeof(size_def) - 1);
		itoa(size_fd, i);
		write(size_fd, equal, sizeof(equal) - 1);
		itoa(size_fd, size[i]);
		write(size_fd, eol, sizeof(eol) - 1);
		/* assignation of sizes[i] = &sizei */
		write(size_fd, sizes, sizeof(sizes) - 1);
		itoa(size_fd, i);
		write(size_fd, size_addr, sizeof(size_addr) - 1);
		itoa(size_fd, i);
		write(size_fd, eol, sizeof(eol) - 1);
		i++;
	}
	write(size_fd, eofunc, sizeof(eofunc) - 1);
}

static int	size_file(int n_files, int *size)
{
	int	size_fd = open("hex_size_file.c", O_CREAT | O_RDWR, 0666);

	if (size_fd == -1)
		return -1;
	write_size_file(size_fd, n_files, size)
	close(size_fd);
}

/* End of the functions to write the sizes of the packed-files */

int mk_hex_files(int n_files, char **files)
{
	int *size = malloc(n_files * sizeof(int));

	if (size == NULL)
		return -1;
	hexa_file(files, n_files, size);
	size_file(n_files, size);
	free(size);
	return 0;
}
