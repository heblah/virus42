#include "headers.h"

/*
* Parse arg error, check if directory, check if binary file or not
* find .text section address in binary
* encrypt .text section
*
*/

int		main(int argc, char **argv)
{
	int	fd;
	// printf("%s",argv[1]);
	if (argc == 2)
	{
		fd = open(argv[1], O_DIRECTORY);
		if (fd != -1)
		{
			perror("open");
			close(fd);
			return 1;
		}
		fd = open(argv[1], O_RDONLY);
		if (fd < 0)
		{
			perror("open");
			close(fd);
			return 1;
		}
		return copy_binary(fd);
		
	}
	printf("Error\nInvalid number of argument\n");
	return 1;
}
