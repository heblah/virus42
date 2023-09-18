#include <stdio.h>
#include <stdlib.h>
#define X 2
#define N(x) (x > 0) ? (x) : (-1 * (x))
#define SRC "#include <stdio.h>%2$c#include <stdlib.h>%2$c#define X %7$d%2$c#define N(x) (x > 0) ? (x) : (-1 * (x))%2$c#define SRC %3$c%6$s%3$c%2$c#define FT(x)int main(void)%5$c%2$c%1$c{%5$c%2$c%1$c%1$cchar filename[50];%5$c%2$c%1$c%1$cchar cmd[3000];%5$c%2$c%1$c%1$cunsigned int n = N(X);%5$c%2$c%1$c%1$cFILE *fd;%5$c%2$c%5$c%2$c%1$c%1$cif (n > 0)%5$c%2$c%1$c%1$c{%5$c%2$c%1$c%1$c%1$csprintf(filename, %3$cSully_%4$cd%4$cs%3$c, n - 1, %3$c.c%3$c);%5$c%2$c%1$c%1$c%1$cfd = fopen(filename, %3$cw+%3$c);%5$c%2$c%1$c%1$c%1$cif (fd == NULL)%5$c%2$c%1$c%1$c%1$c%1$creturn (0);%5$c%2$c%1$c%1$c%1$cfprintf(fd, SRC, 9, 10, 34, 37, 92, SRC, n - 1);%5$c%2$c%1$c%1$c%1$cfclose(fd);%5$c%2$c%1$c%1$c%1$csprintf(filename, %3$cSully_%4$cd%3$c, n - 1);%5$c%2$c%1$c%1$c%1$csprintf(cmd, %3$cgcc %4$c1$s%4$c2$s -o %4$c1$s && ./%4$c1$s%3$c, filename, %3$c.c%3$c);%5$c%2$c%1$c%1$c%1$cif (n - 1 > 0)%5$c%2$c%1$c%1$c%1$c%1$csystem(cmd);%5$c%2$c%1$c%1$c}%5$c%2$c%1$c%1$creturn (0);%5$c%2$c%1$c}%2$cFT(x)%2$c"
#define FT(x)int main(void)\
	{\
		char filename[50];\
		char cmd[3000];\
		unsigned int n = N(X);\
		FILE *fd;\
\
		if (n > 0)\
		{\
			sprintf(filename, "Sully_%d%s", n - 1, ".c");\
			fd = fopen(filename, "w+");\
			if (fd == NULL)\
				return (0);\
			fprintf(fd, SRC, 9, 10, 34, 37, 92, SRC, n - 1);\
			fclose(fd);\
			sprintf(filename, "Sully_%d", n - 1);\
			sprintf(cmd, "gcc %1$s%2$s -o %1$s && ./%1$s", filename, ".c");\
			if (n - 1 > 0)\
				system(cmd);\
		}\
		return (0);\
	}
FT(x)
