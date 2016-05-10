/*
 * File: symbols.h
 * ----------------
 * This is where you declare any exported features of the symbols.c
 * module.  As is good header file practice, you should take care to
 * set the header up to work correctly even if included more than once.
 */

#ifndef _symbols_h
#define _symbols_h

int ObjectFileOpen(const char *filename);
void PrintSymtab(void);
char * SearchSymbol(const char *address, long long int *offset);
void ObjectFileClose(void);


#endif //end of _symbols_h
