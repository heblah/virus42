#include <stdio.h>
#define FT_FILE "./Grace_kid.c"
#define TXT(y) "#include <stdio.h>%2$c#define FT_FILE %3$c./Grace_kid.c%3$c%2$c#define TXT(y) %3$c%5$s%3$c%2$c#define FT(x)int main(void)%4$c%2$c%1$c{%4$c%2$c%1$c%1$cFILE *fd;%4$c%2$c%1$c%1$cfd = fopen(FT_FILE, %3$cw+%3$c);%4$c%2$c%1$c%1$cif (fd == NULL)%4$c%2$c%1$c%1$c%1$creturn (0);%4$c%2$c%1$c%1$cfprintf(fd, TXT(y), 9, 10, 34, 92, TXT(y));%4$c%2$c%1$c%1$creturn (0);%4$c%2$c%1$c}%2$cFT(x)%2$c/*%2$c%1$cbla bla bla%2$c*/%2$c"
#define FT(x)int main(void)\
	{\
		FILE *fd;\
		fd = fopen(FT_FILE, "w+");\
		if (fd == NULL)\
			return (0);\
		fprintf(fd, TXT(y), 9, 10, 34, 92, TXT(y));\
		return (0);\
	}
FT(x)
/*
	bla bla bla
*/
