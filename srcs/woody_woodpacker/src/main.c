
#define N_FILES 13

#include <unistd.h>
#include <sys/syscall.h>

#include "woody.h"

int	main(int argc, char **argv, char **env)
{
	char usage[] = "usage:\n./woodywood_packer target_1 target_2 ... target_n with 0 < n <= 13\n";
	char *cmd[] = {"/bin/gcc", "-Wall", "-Wextra", "-Werror", "-g3", "src/load_elf.c", "src/hex_content_file.c", "src/hex_size_file.c", "-o", "woody", NULL};

	if (argc - 1 == 0 || argc - 1 > N_FILES)
	{
		write(2, usage, sizeof(usage) - 1);
		return -1;
	}
	mk_hex_files(argc - 1, argv + 1);
	syscall(SYS_execve, cmd[0], cmd, env);
	return 0;
}
