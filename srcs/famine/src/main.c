
/*
 * Project      : Famine
 * Author       : halvarez / plgueugnon
 * Starting date: 01/12/2023
 */

#include <stdio.h>

#include "famine.h"

int	main(int argc, char **argv)
{
	t_target	target;
	t_payload	payload;

	if (argc != 3)
	{
#if DEBUG
		fprintf(stderr, "Usage: %s elf_file payload\n", argv[0]);
#endif
		return 1;
	}
	target.info.fname = argv[1];
	paryload.info.fname = argv[2];

	/* Processing target */
	target.info.fd = elf_open_and_map(&target.info);
	target.elf_hdr = (Elf64_Ehdr *)target.info.data;
	target_entrypoint = elf_hdr->e_entry;
#if DEBUG
	printf("+ Target entrypoint: %p\n", (void *)target_entrypoint);
#endif
	target_text_segment = elf_find_gap(target_data, target_fsize, &p, &len);

	/* Payload part */
	payload_fd = elf_open_and_map(argv[2], &payload_data, &payload_fsize);
	return 0;
}
