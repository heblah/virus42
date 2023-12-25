
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/memfd.h>
#include <fcntl.h>

int main() {
    const char *elf_content = "/elf"; // Replace with your ELF content
	int fd = open("elf", O_RDWR);
	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	off_t fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
    // Create an anonymous file in memory
    int memfd = syscall(SYS_memfd_create, "my_executable", MFD_CLOEXEC);
    if (memfd == -1) {
        perror("Error creating anonymous file");
        exit(EXIT_FAILURE);
    }

    // Write ELF content to the anonymous file
    if (write(memfd, elf_content, strlen(elf_content)) == -1) {
        perror("Error writing to anonymous file");
        close(memfd);
        exit(EXIT_FAILURE);
    }

    // Seek to the beginning of the file
    lseek(memfd, 0, SEEK_SET);

    // Execute the anonymous file
    char memfd_path[20];
    snprintf(memfd_path, sizeof(memfd_path), "/proc/self/fd/%d", memfd);

    char *const argv[] = {memfd_path, NULL};
    char *const envp[] = {NULL};

    if (execve(memfd_path, argv, envp) == -1) {
        perror("Error executing anonymous file");
        close(memfd);
        exit(EXIT_FAILURE);
    }

    close(memfd);
    return 0;
}
