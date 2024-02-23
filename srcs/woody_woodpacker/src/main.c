
#include "woody.h"
#define N_FILES 13
#include <unistd.h>

int	main(int argc __attribute((unused)), char **argv __attribute__((unused)))
{
	char usage[] = "usage:\n./woodywood_packer target_1 target_2 ... target_n with 0 < n <= 13\n";

	if (argc - 1 == 0 || argc - 1 > N_FILES)
	{
		write(2, usage, sizeof(usage) - 1);
		return -1;
	}
	mk_hex_files(argc - 1, argv + 1);
	return 0;
}
