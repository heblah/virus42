#include <stdio.h>

/*
	My first comment
*/

void	function(void);

int	main()
{
	char *s= "#include <stdio.h>%c%c/*%c%cMy first comment%c*/%c%cvoid%cfunction(void);%c%cint%cmain()%c{%c%cchar *s= %c%s%c;%c%c/*%c%cMy second comment%c*/%c%c%cprintf(s, 10, 10, 10, 9, 10, 10, 10, 9, 10, 10, 9, 10, 10, 9, 34, s, 34, 10, 10, 10, 9, 10, 10, 10, 9, 10, 9, 10, 9, 10, 10, 10, 9, 10, 10, 10);%c%cfunction();%c%creturn (0);%c}%c%cvoid%cfunction(void)%c{%c}%c";

/*
	My second comment
*/

	printf(s, 10, 10, 10, 9, 10, 10, 10, 9, 10, 10, 9, 10, 10, 9, 34, s, 34, 10, 10, 10, 9, 10, 10, 10, 9, 10, 9, 10, 9, 10, 10, 10, 9, 10, 10, 10);
	function();
	return (0);
}

void	function(void)
{
}
