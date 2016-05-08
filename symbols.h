/*
 * File: symbols.h
 * ----------------
 * This is where you declare any exported features of the symbols.c
 * module.  As is good header file practice, you should take care to
 * set the header up to work correctly even if included more than once.
 */

void PrintSymtab(const char *filename);
void SearchSymbol(const char *filename, const char *address);
