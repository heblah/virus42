
/*
 * Project      : Famine
 * Author       : halvarez / plgueugnon
 * Starting date: 01/12/2023
 */

#include <stdio.h>

#include "famine.h"

int	main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	void *data_target = NULL, *data_payload = NULL;
	int target_fd = -1, payload_fd = -1;
	long fsize_target = 0, fsize_payload = 0;

	if (argc != 3)
	{
#if DEBUG
		fprintf(stderr, "Usage: %s elf_file payload\n", argv[0]);
#endif
		return 1;
	}

	target_fd = elf_open_and_map(argv[1], data_target, &fsize_target);
	payload_fd = elf_open_and_map(argv[2], data_payload, &fsize_payload);
	return 0;
}
