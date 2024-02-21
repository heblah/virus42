
#include "woody.h"
#define N_FILES 10
#include <unistd.h>

int	main(int argc __attribute((unused)), char **argv __attribute__((unused)))
{
	if (argc - 1 > N_FILES)
		return -1;
	mk_hex_files(argc - 1, argv + 1);
	return 0;
}
