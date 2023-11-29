
#include <unistd.h>
#include <stdio.h>

int main(void)
{
	printf("Here prog 1 with uid = %d\n", geteuid());
	return 0;
}
