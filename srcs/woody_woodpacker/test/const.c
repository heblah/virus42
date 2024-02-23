
#include<stdio.h>

int main()
{
	const int n __attribute__((unused)) = 8;
	int * const ptr = (int *)&n;

	printf("n = %d\n", n);
	printf("n = %d\n", n);
	*ptr += 1;
	printf("*ptr = %d\n", *ptr);

	return 0;
}
