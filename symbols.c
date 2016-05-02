/* file: symbols.c
 * ---------------
 * Thie file will eventually contain the code to extract the symbol table info
 * from an Elf file.  Right now, it just contains the Elf struct defintions and
 * a little starting code to show you how to memory-map a file.
 */
#include <elf.h>
#include <string.h>		// memcmp
#include <fcntl.h>		// open
#include <sys/mman.h>	        // mmap
#include <unistd.h>		// lseek
#include "symbols.h"


/* 
 * The <elf.h> header already declares structs for the file header, section
 * header, symbols, etc.  but the naming conventions are cryptic and
 * multi-layered, which makes it a hassle to try to figure out what a
 * particular field/type boils down to, so we declare our own version of these
 * structures with simpler names to keep things cleaner for you.  We do
 * directly use the #define-d ELF constants such as SHT_SYMTAB, SHT_STRTAB,
 * STT_FUNC which come from <elf.h>
 */
 
/* Type: Elf_File_Header
 * ---------------------
 * A struct that describes the data found in first few bytes of Elf file.
 */
typedef struct {
   char	identity[16];		 // ELF specification information
   int	other_ints[4];           // you can ignore these fields
   int	offset_to_section_header_table; // offset in bytes from start of file to section headers
   short other_shorts[6];        // you can ignore these fields
   short number_of_section_headers; // count of section headers in table
} Elf_File_Header;


/* Type: Elf_Section_Header
 * ------------------------
 * A struct that describes the data for each section header. The section
 * header table is a contiguous array of section header structs.
 */
typedef struct {
   int	name;
   int	type;		// type of section: SHT_SYMTAB, SHT_STRTAB, SHT_REL, etc.
   int	flags;
   int	addr; 
   int	offset;		// offset in bytes from file begin to where section data starts
   int	size;		// number of bytes of data in the section
   int	strtab_index;	// index into section header table for associated string table section
   int	other_ints[3];
} Elf_Section_Header;


/* Type: Elf_Symbol
 * ----------------
 * A struct that describes the data for each symbol. The symtab
 * section is a contiguous array of symbol structs.
 */
typedef struct {
   int   name;			// offset into string table for symbol name
   int	 address;		// symbol address
   int	 size;			// symbol size in bytes 
   char	 type_and_binding;      // low-order 4 bits are type (STT_FUNC, STT_OBJECT)
                                // high-order 4 bits are binding (STB_LOCAL, STB_GLOBAL)
   char reserved;
   char section_tag;            // will be SHN_UNDEF if symbol is undefined
} Elf_Symbol;




/* Function: GetElfData
 * ---------------------
 * This function is given a pathname to an object/executable file. It will
 * memory-map map the entire file and return a pointer to memory containing the
 * file data in one contiguous block. As a sanity check, it verifies that the
 * identity bytes at the start of the file indicate a proper Elf header.  The
 * second parameter is pased by reference and is filled in with the number of
 * bytes in the mapped data. When done with this data, you should call the
 * DiposeElfData function on the pointer and its size to release the memory.
 * If the file couldn't be opened or didn't have a proper Elf header, NULL is
 * returned and numBytes is set to zero.
 */
static void *GetElfData(const char *filename, int *numBytes)
{
   char ELF_IDENTITY[] = { 0x7f, 'E', 'L', 'F', ELFCLASS32, ELFDATA2LSB, EV_CURRENT};
   int fd;
   *numBytes = 0;
   if ((fd = open(filename, O_RDONLY)) == -1)
      return NULL;
   int file_size = lseek(fd, 0, SEEK_END);
   void *data = mmap(0, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
   close(fd);
   if (data == MAP_FAILED) return NULL;

   // file header is at offset 0 within file, use typecast at this location to
   // access header
   Elf_File_Header *hdr = (Elf_File_Header *)data; 
   if (memcmp(hdr->identity, ELF_IDENTITY, sizeof(ELF_IDENTITY)) != 0)
      return NULL; // bail if start of file doesn't indicate correct 32-bit Elf file header
   *numBytes = file_size;
   return data;
}


static void DisposeElfData(void *data, int size)
{
   munmap(data, size);
}

