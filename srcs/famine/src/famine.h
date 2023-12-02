
#ifndef FAMINE_H
# define FAMINE_H

#ifndef DEBUG
# define DEBUG 0
#endif

int	elf_open_and_map(char *fname, void **data, long *len);
Elf64_Phdr *	elf_find_gap(void *data, int fsize, int *p, long *len);


#endif
