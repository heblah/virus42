
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	open("/test", O_CREAT | O_RDWR);
	return 0;
}
