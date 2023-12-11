
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
typedef struct s_target
{
	t_info		elf;
	Elf64_Ehdr	*Ehdr;
	Elf64_Phdr	*text_segment;
	Elf64_Addr	e_entry;
	int			len;
	int			base;
	long		text_end;
	long		gap;
} t_target;

/* 64 bits payload structure */
typedef struct s_payload
{
	t_info		elf;
	Elf64_Shdr	text_section;
}	t_payload;

int				elf_open_and_map(t_info *elf);
Elf64_Phdr *	elf_find_gap(t_target *target);
void			draw_elf(Elf64_Ehdr *ehdr);


#endif
