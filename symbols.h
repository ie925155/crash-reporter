/*
 * File: symbols.h
 * ----------------
 * This is where you declare any exported features of the symbols.c
 * module.  As is good header file practice, you should take care to
 * set the header up to work correctly even if included more than once.
 */

void *GetElfData(const char *filename, int *numBytes);
void PrintSymtab(void *elfData);
void SearchSymbol(void *elfData, char *address);
void DisposeElfData(void *data, int size);
