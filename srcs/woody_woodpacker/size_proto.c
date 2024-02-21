
#define N_FILES n

int get_hex_size(int i)
{
	static int *sizes[N_FILES];
	static int size0 = /*...*/;
	sizes[0] = &size0;
	static int size1 = /*...*/;
	sizes[1] = &size1;
	//...
	return i < N_FILES ? sizes[i] : -1;
}
