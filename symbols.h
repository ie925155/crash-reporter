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
void SearchSymbol(const char *address);
void ObjectFileClose(void);


#endif //end of _symbols_h
