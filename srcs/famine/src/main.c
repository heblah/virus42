
/*
 * Project      : Famine
 * Author       : halvarez / plgueugnon
 * Starting date: 01/12/2023
 */

#include <stdio.h>

#include "famine.h"

int	main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	t_elf	target;
	t_elf	payload;

	/*
	if (argc != 3)
	{
#if DEBUG
		fprintf(stderr, "Usage: %s elf_file payload\n", argv[0]);
#endif
		return 1;
	}
	*/
	target.fname = argv[1];
	payload.fname = argv[2];

	/* Processing target */
	target.fd = elf_open_and_map(&target);
	elf_x_section_counter(target.shdr, target.ehdr->e_shnum);
	print_headers(&target);
	//elf_find_gap(&target);

	/*
#if DEBUG
	printf("+ Target entrypoint: %p\n", (void *)target_entrypoint);
#endif
	target_text_segment = elf_find_gap(target_data, target_fsize, &p, &len);

	// Payload part
	payload_fd = elf_open_and_map(argv[2], &payload_data, &payload_fsize);
	return 0;
	*/
}
