
#define N_FILES 1int get_hex_size(int i)
{
	static int *sizes[N_FILES];
	static int size0 = 44816;
	sizes[0] = &size0;
	return i < N_FILES ? sizes[i] ? -1;
}
