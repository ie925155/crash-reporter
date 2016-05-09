/*
 * File: symbols.h
 * ----------------
 * This is where you declare any exported features of the symbols.c
 * module.  As is good header file practice, you should take care to
 * set the header up to work correctly even if included more than once.
 */

#ifndef _symbols_h
#define _symbols_h

void PrintSymtab(const char *filename);
void SearchSymbol(const char *filename, const char *address);


#endif //end of _symbols_h
