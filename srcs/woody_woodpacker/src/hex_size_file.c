
#define N_FILES 3

int get_hex_size(int i)
{
	static int sizes[N_FILES];
	static int size0 = 44816;
	sizes[0] = size0;
	static int size1 = 44824;
	sizes[1] = size1;
	static int size2 = 44824;
	sizes[2] = size2;
	return i < N_FILES ? sizes[i] : -1;
}

int get_n_files(void) 
{
	return N_FILES;
}
