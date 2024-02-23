
#include "woody.h"
#define N_FILES 13
#include <unistd.h>
#include <stdlib.h>

int	main(int argc __attribute((unused)), char **argv __attribute__((unused)))
{
	char usage[] = "usage:\n./woodywood_packer target_1 target_2 ... target_n with 0 < n <= 13\n";
	//char *cmd[5];
	//char cc[] = "/bin/gcc";
	//char flags[] = "-Wall -Wextra -Werror src/load_elf.c src/hex_content_file.c src/hex_size_file.c -o woody";
	//char files[] = "src/load_elf.c src/hex_content_file.c src/hex_size_file.c";
	//char out[] = "-o woody";

	//cmd[0] = cc;
	//cmd[1] = flags;
	//cmd[2] = NULL;
	//cmd[3] = NULL;
	//cmd[4] = NULL;
	if (argc - 1 == 0 || argc - 1 > N_FILES)
	{
		write(2, usage, sizeof(usage) - 1);
		return -1;
	}
	mk_hex_files(argc - 1, argv + 1);
	system("gcc -Wall -Wextra -Werror src/load_elf.c src/hex_content_file.c src/hex_size_file.c -o woody");
	//execve(cc, cmd, NULL);
	return 0;
}
