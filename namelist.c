#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "symbols.h"

int main(int argc, char *argv[])
{
	int data_size;
	void *data_ptr;
	if(argc < 2)
		fprintf(stderr, "You have to type file name\n");
	data_ptr = GetElfData(argv[1], &data_size);
	if(data_ptr == NULL) return 0;
	PrintSymtab(data_ptr);
	DisposeElfData(data_ptr, data_size);
	return 0;
}
