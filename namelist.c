#include <stdio.h>
#include "symbols.h"

int main(int argc, char *argv[])
{
	int ret;
	int data_size;
	void *data_ptr;

	if(argc < 2)
		fprintf(stderr, "You have to type file name\n");

	data_ptr = GetElfData(argv[1], &data_size);
	return 0;
}
