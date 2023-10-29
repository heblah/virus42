#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
* Parse arg error, check if directory, check if binary file or not
* find .text section address in binary
* encrypt .text section
*
*/

int		main(int argc, char **argv)
{
	int	fd;
	if (argc == 2)
	{
		fd = open(argv[1], O_DIRECTORY);
		if (fd != -1)
		{
			printf("Error\nFirst argument is a directory\n");
			close(fd);
			exit(0);
		}
	}
	printf("Error\nInvalid number of argument\n");
	return 0;
}
