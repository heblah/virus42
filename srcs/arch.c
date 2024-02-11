
// N_FILES number of files
#define N_FILES 5

const unsigned char *get_content(int i)
{
	static unsigned char *index[N_FILES];
	static unsigned char file0[] = {
		/* hexadecimal conetent */
	};
	static unsigned char file1[] = {
		/* hexadecimal conetent */
	};
	//...

	index[0] = file0;
	index[1] = file1;
	//...
	return i < N_FILES ? index[i] : 0;
}

const int get_size(int i)
{
	static int index[N_FILES];
	static int size0;
	static int size1;
	//...

	index[0] = size0;
	index[1] = size1;
	//...
	return i < N_FILES ? index[i] : -1;
}
