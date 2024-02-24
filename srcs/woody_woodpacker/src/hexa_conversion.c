
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define HEX_SIZE_FILE "src/hex_size_file.c"
#define HEX_CONTENT_FILE "src/hex_content_file.c"

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

static int	write_hex_content(int content_fd, int content_size, int index, int src_fd)
{
	char conv_tab[][5] = {
		"0x00", "0x01", "0x02", "0x03", "0x04", "0x05", "0x06", "0x07", "0x08", "0x09",
		"0x0a", "0x0b", "0x0c", "0x0d", "0x0e", "0x0f", "0x10", "0x11", "0x12", "0x13",
		"0x14", "0x15", "0x16", "0x17", "0x18", "0x19", "0x1a", "0x1b", "0x1c", "0x1d",
		"0x1e", "0x1f", "0x20", "0x21", "0x22", "0x23", "0x24", "0x25", "0x26", "0x27",
		"0x28", "0x29", "0x2a", "0x2b", "0x2c", "0x2d", "0x2e", "0x2f", "0x30", "0x31",
		"0x32", "0x33", "0x34", "0x35", "0x36", "0x37", "0x38", "0x39", "0x3a", "0x3b",
		"0x3c", "0x3d", "0x3e", "0x3f", "0x40", "0x41", "0x42", "0x43", "0x44", "0x45",
		"0x46", "0x47", "0x48", "0x49", "0x4a", "0x4b", "0x4c", "0x4d", "0x4e", "0x4f",
		"0x50", "0x51", "0x52", "0x53", "0x54", "0x55", "0x56", "0x57", "0x58", "0x59",
		"0x5a", "0x5b", "0x5c", "0x5d", "0x5e", "0x5f", "0x60", "0x61", "0x62", "0x63",
		"0x64", "0x65", "0x66", "0x67", "0x68", "0x69", "0x6a", "0x6b", "0x6c", "0x6d",
		"0x6e", "0x6f", "0x70", "0x71", "0x72", "0x73", "0x74", "0x75", "0x76", "0x77",
		"0x78", "0x79", "0x7a", "0x7b", "0x7c", "0x7d", "0x7e", "0x7f", "0x80", "0x81",
		"0x82", "0x83", "0x84", "0x85", "0x86", "0x87", "0x88", "0x89", "0x8a", "0x8b",
		"0x8c", "0x8d", "0x8e", "0x8f", "0x90", "0x91", "0x92", "0x93", "0x94", "0x95",
		"0x96", "0x97", "0x98", "0x99", "0x9a", "0x9b", "0x9c", "0x9d", "0x9e", "0x9f",
		"0xa0", "0xa1", "0xa2", "0xa3", "0xa4", "0xa5", "0xa6", "0xa7", "0xa8", "0xa9",
		"0xaa", "0xab", "0xac", "0xad", "0xae", "0xaf", "0xb0", "0xb1", "0xb2", "0xb3",
		"0xb4", "0xb5", "0xb6", "0xb7", "0xb8", "0xb9", "0xba", "0xbb", "0xbc", "0xbd",
		"0xbe", "0xbf", "0xc0", "0xc1", "0xc2", "0xc3", "0xc4", "0xc5", "0xc6", "0xc7",
		"0xc8", "0xc9", "0xca", "0xcb", "0xcc", "0xcd", "0xce", "0xcf", "0xd0", "0xd1",
		"0xd2", "0xd3", "0xd4", "0xd5", "0xd6", "0xd7", "0xd8", "0xd9", "0xda", "0xdb",
		"0xdc", "0xdd", "0xde", "0xdf", "0xe0", "0xe1", "0xe2", "0xe3", "0xe4", "0xe5",
		"0xe6", "0xe7", "0xe8", "0xe9", "0xea", "0xeb", "0xec", "0xed", "0xee", "0xef",
		"0xf0", "0xf1", "0xf2", "0xf3", "0xf4", "0xf5", "0xf6", "0xf7", "0xf8", "0xf9",
		"0xfa", "0xfb", "0xfc", "0xfd", "0xfe", "0xff"
	};
	int32_t byte = 0;
	uint8_t *buffer = malloc(content_size);
	char	name[] = "\tstatic unsigned char file";
	char	brackets[] = "[] = {\n\t\t";

	if (buffer == NULL || read(src_fd, buffer, content_size) != content_size)
		return -1;
	write(content_fd, name, sizeof(name) - 1);
	itoa(content_fd, index);
	write(content_fd, brackets, sizeof(brackets) - 1);
	while (byte < content_size)
	{
		/*
		 * XORing only certain bytes for encryption:
		 *  -Easy to use, complicate to decrypt
		 *  -byte % 2 == 0 || byte % 7 == 0 || byte % 11 == 0 || byte % 13 == 0
		 */
		if (byte % 2 == 0 || byte % 7 == 0 || byte % 11 == 0 || byte % 13 == 0)
			buffer[byte] ^= 0xff;
		write(content_fd, conv_tab[buffer[byte]], 4);
		if ((byte + 1) % 12 == 0 && byte != content_size - 1)
			write(content_fd, ",\n\t\t", 4);
		else if (byte != content_size - 1)
			write(content_fd, ", ", 2);
		byte++;
	}
	write(content_fd, "\n\t};", 4);
	free(buffer);
	return 0;
}

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
	int content_fd = open(HEX_CONTENT_FILE, O_CREAT | O_RDWR, 0666);

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
			return -1;
		//bufferize
		//encryption
		write_hex_content(content_fd, size[i], i, src_fd);
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
	char func[] = "\n\nint get_hex_size(int i)\n{\n\tstatic int sizes[N_FILES];\n";
	char size_def[] = "\tstatic int size";
	char size_addr[] = "] = size";
	char sizes[] = "\tsizes[";
	char equal[] = " = ";
	char eol[] = ";\n";
	char eofunc[] = "\treturn i < N_FILES ? sizes[i] : -1;\n}\n";
	char get_n_files[] = "\nint get_n_files(void) \n{\n\treturn N_FILES;\n}\n";
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
	write(size_fd, get_n_files, sizeof(get_n_files) - 1);
}

static int	size_file(int n_files, int *size)
{
	int	size_fd = open(HEX_SIZE_FILE, O_CREAT | O_RDWR, 0666);

	if (size_fd == -1)
		return -1;
	write_size_file(size_fd, n_files, size);
	close(size_fd);
	return 0;
}

/* End of the functions to write the sizes of the packed-files */

int mk_hex_files(int n_files, char **files)
{
	int *size = malloc(n_files * sizeof(int));

	if (size == NULL)
		return -1;
	syscall(SYS_unlink, HEX_SIZE_FILE);
	syscall(SYS_unlink, HEX_CONTENT_FILE);
	if (hexa_file(files, n_files, size) == -1)
	{
		syscall(SYS_unlink, HEX_SIZE_FILE);
		free(size);
		return -1;
	}
	if (size_file(n_files, size) == -1)
	{
		syscall(SYS_unlink, HEX_SIZE_FILE);
		syscall(SYS_unlink, HEX_CONTENT_FILE);
		free(size);
		return -1;
	}
	free(size);
	return 0;
}
