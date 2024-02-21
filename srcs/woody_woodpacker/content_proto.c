
#define N_FILES n

const unsigned char *get_hex_content(int i)
{
	static unsigned char file0[] = {
		/* hexa representation */
	};
	static unsigned char file1[] = {
		/* hexa representation */
	};
	//...
	static unsigned char *files[N_FILES];
	files[0] = file0;
	files[1] = file1;
	//...
	return i < N_FILES ? files[i] : 0;
}
