#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "symbols.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
		fprintf(stderr, "You have to type file name\n");
	ObjectFileOpen(argv[1]);
	long long int offset;
	if(argc == 2)
	  PrintSymtab();
	else if(argc == 3)
		SearchSymbol(argv[2], &offset);
	ObjectFileClose();
	return 0;
}
