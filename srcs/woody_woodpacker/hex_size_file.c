
#define N_FILES 0

int get_hex_size(int i)
{
	static int sizes[N_FILES];
	return i < N_FILES ? sizes[i] : -1;
}

int get_n_files(void) 
{
	return N_FILES;
}
