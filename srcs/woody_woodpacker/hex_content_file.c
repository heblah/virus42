
#define N_FILES 1

const unsigned char *get_hex_content(int i)
{
	static unsigned char *files[N_FILES];
	files[0] = file0;
	return i < N_FILES ? files[i] : 0;
}
