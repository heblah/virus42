
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
	int fd = open("/Hello", O_CREAT | O_RDWR);
	if (fd == -1)
		printf("File creation failed\n");
	else
		printf("File created\n");
	return 0;
}
