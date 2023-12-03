
#ifndef FAMINE_H
# define FAMINE_H

#ifndef DEBUG
# define DEBUG 0
#endif

typdef struct s_info
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
	Elf64_hdr	*elf_hdr;
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

int	elf_open_and_map(char *fname, void **data, long *len);
Elf64_Phdr *	elf_find_gap(void *data, int fsize, int *p, long *len);


#endif
