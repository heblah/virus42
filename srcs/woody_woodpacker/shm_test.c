
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void)
{
	char file[] = "in-mem";
	int fd = shm_open(file, O_CREAT | O_RDWR, 0666);
	printf("fd = %d\n", fd);
	printf("pid = %d\n", getpid());
	system("ls /dev/shm/");
	shm_unlink(file);
	shm_unlink("inmem");
	return 0;
}
