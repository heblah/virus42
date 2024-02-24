
#define N_FILES 1

int get_hex_size(int i)
{
	static int sizes[N_FILES];
	static int size0 = 108976;
	sizes[0] = size0;
	return i < N_FILES ? sizes[i] : -1;
}

int get_n_files(void) 
{
	return N_FILES;
}
