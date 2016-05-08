#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "symbols.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
		fprintf(stderr, "You have to type file name\n");
	if(argc == 2)
	  PrintSymtab(argv[1]);
	else if(argc == 3)
		SearchSymbol(argv[1], argv[2]);
	return 0;
}
