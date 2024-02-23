
#define N_FILES 0

const unsigned char *get_hex_content(int i)
{
	static unsigned char *files[N_FILES];
	return i < N_FILES ? files[i] : 0;
}
