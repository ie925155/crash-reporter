#
# Makefile for CSE333 assign
#

# It is likely that default C compiler is already gcc, but explicitly
# set, just to be sure
CC = gcc

# The CFLAGS variable sets compile flags for gcc:
#  -g          compile with debug information
#  -Wall       provide diagnostic warnings
#  -O0         do not optimize generated code
#  -std=gnu99  use the Gnu C99 standard language definition
#  -m32        emit code for IA32 architecture
CFLAGS = -g -Wall -O0 -std=gnu11 -m64 

# The LDFLAGS variable sets flags for linker
#  -m32  link with 32-bit libraries
LDFLAGS =  -m64

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.
HEADERS = reporter.h symbols.h
SOURCES = namelist.c reporter.c symbols.c buggy.c
OBJECTS = $(SOURCES:.c=.o)
TARGETS = libreporter.a namelist buggy

default: $(TARGETS)

# The first target defined in the makefile is the one
# used when make is invoked with no argument. Given the definitions
# above, this Makefile file will build all three targets.

namelist : namelist.o symbols.o
	$(CC) $(CFLAGS)  -o $@ $^ $(LDFLAGS) -L/mnt/hgfs/glay_luncy/Dropbox/cs107/crash-reporter/hw7 -lccontainer -static

libreporter.a : reporter.o symbols.o
	ar rcs $@ $^

buggy : buggy.o libreporter.a
	$(CC) $(CFLAGS) -o $@ $^ -static

# In make's default rules, a .o automatically depends on its .c file
# (so editing the .c will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .c to be recompiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" that is used to remove all compiled object files.

.PHONY: clean

clean:
	@rm -f $(TARGETS) *.o core Makefile.dependencies sanity_buggy*
