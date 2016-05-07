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
#include <assert.h>		// lseek
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "symbols.h"
#include "cvector.h"

/*
 * The <elf.h> header already declares structs for the file header, section
 * header, symbols, etc.  but the naming conventions are cryptic and
 * multi-layered, which makes it a hassle to try to figure out what a
 * particular field/type boils down to, so we declare our own version of these
 * structures with simpler names to keep things cleaner for you.  We do
 * directly use the #define-d ELF constants such as SHT_SYMTAB, SHT_STRTAB,
 * STT_FUNC which come from <elf.h>
 */

/* Type: Elf64_File_Header
 * ---------------------
 * A struct that describes the data found in first few bytes of Elf file.
 */
typedef struct {
   char	identity[16];		 // ELF specification information
   int	other_ints[6];           // you can ignore these fields
   long	offset_to_section_header_table; // offset in bytes from start of file to section headers
   short other_shorts[6];        // you can ignore these fields
   short number_of_section_headers; // count of section headers in table
   short other_short[1];
} Elf64_File_Header;


/* Type: Elf64_Section_Header
 * ------------------------
 * A struct that describes the data for each section header. The section
 * header table is a contiguous array of section header structs.
 */
typedef struct {
   int	name;
   int	type;		         // type of section: SHT_SYMTAB, SHT_STRTAB, SHT_REL, etc.
   long	flags;
   long	addr;
   long	offset;		       // offset in bytes from file begin to where section data starts
   long	size;		         // number of bytes of data in the section
   int	strtab_index;	   // index into section header table for associated string table section
   int other_int;
   long	other_ints[2];
} Elf64_Section_Header;


/* Type: Elf64_Symbol
 * ----------------
 * A struct that describes the data for each symbol. The symtab
 * section is a contiguous array of symbol structs.
 */
typedef struct {
   int   name;			            // offset into string table for symbol name
   int	 address;		            // symbol address
   int	 size;			            // symbol size in bytes
   char	 type_and_binding;      // low-order 4 bits are type (STT_FUNC, STT_OBJECT)
                                // high-order 4 bits are binding (STB_LOCAL, STB_GLOBAL)
   char reserved;
   char section_tag;            // will be SHN_UNDEF if symbol is undefined
} Elf64_Symbol;

typedef struct {
  char **name;
  Elf64_Addr address;
  Elf64_Xword size;
  uint8_t binding;
} SYMBOL_INFO;


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
void *GetElfData(const char *filename, int *numBytes)
{
   char ELF_IDENTITY[] = { 0x7f, 'E', 'L', 'F', ELFCLASS64, ELFDATA2LSB, EV_CURRENT};
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
   Elf64_Ehdr *hdr = (Elf64_Ehdr *)data;
   if (memcmp(hdr->e_ident, ELF_IDENTITY, sizeof(ELF_IDENTITY)) != 0)
      return NULL; // bail if start of file doesn't indicate correct 32-bit Elf file header
   *numBytes = file_size;
   return data;
}

int symtab_compare(const void *elemAddr1, const void *elemAddr2)
{
  SYMBOL_INFO *symbol_info1 = (SYMBOL_INFO*) elemAddr1;
  SYMBOL_INFO *symbol_info2 = (SYMBOL_INFO*) elemAddr2;
  return strcasecmp(*(const char **)symbol_info1->name,
    *(const char **)symbol_info2->name);
}

void PrintSymtab(void *elfData)
{
  uint8_t *strtab_ptr = NULL;
  Elf64_Sym *symtab_ptr = NULL;
  uint32_t num_of_symbols = 0;
  assert(elfData != NULL);
  Elf64_Ehdr *hdr = (Elf64_Ehdr *)elfData;      //point ELF Header
  Elf64_Shdr *sh_ptr;
  CVector *vector_symtab;
  for(int i = 0 ; i < hdr->e_shnum ; i++)
  {
    //printf("type = %d\n", (sh_ptr + i)->sh_type);
    //access every section entry
    sh_ptr = (Elf64_Shdr*)((uint8_t*)elfData + hdr->e_shoff) + i;

    if((sh_ptr)->sh_type == SHT_STRTAB) //find the strtab entry
    {
      strtab_ptr = (uint8_t*)elfData + (sh_ptr)->sh_offset;
      //printf("strtab_ptr = %s\n", strtab_ptr + (sh_ptr)->sh_name);
      if(memcmp(strtab_ptr + (sh_ptr)->sh_name, ".shstrtab", sizeof(".shstrtab")) == 0)
        strtab_ptr = NULL;
    }
    if((sh_ptr)->sh_type == SHT_SYMTAB) //find symtab entry
    {
      symtab_ptr = (Elf64_Sym*) ((uint8_t*)elfData + (sh_ptr)->sh_offset);
      num_of_symbols = (sh_ptr)->sh_size / (sh_ptr)->sh_entsize;
      //printf("sh_size = %" PRIu64 "\n", (sh_ptr)->sh_size);
      //printf("sh_entsize = %" PRIu64 "\n", (sh_ptr)->sh_entsize);
    }
  }
  vector_symtab = CVectorCreate(sizeof(SYMBOL_INFO), num_of_symbols, NULL);
  char **keep_ptr = malloc(sizeof(char*)* num_of_symbols);
  int index = 0;
  //dissect symtab section
  Elf64_Sym *symtab_index;
  for(int i = 0 ; i < num_of_symbols ; i++)
  {
    symtab_index = (symtab_ptr + i);
    if((symtab_ptr + i)->st_name > 0)
    {
      SYMBOL_INFO symbol_info;
      //printf("offset = %s \n", strtab_ptr + symtab_index->st_name);
      //const char *symtab_name = (const char*)strtab_ptr + symtab_index->st_name;
      keep_ptr[index++] = (char*)strtab_ptr + symtab_index->st_name;
      uint8_t binding = symtab_index->st_info >> 4;
      uint8_t type = (symtab_index->st_info & 0x0F);
      Elf64_Half st_shndx = symtab_index->st_shndx;
      Elf64_Addr st_value = symtab_index->st_value;
      Elf64_Xword st_size = symtab_index->st_size;
      if(st_shndx != SHN_UNDEF && type == STT_FUNC && (binding == STB_GLOBAL
        || binding == STB_LOCAL))
      {
        /*printf("%016lx %016lx %c %s\n", st_value, st_size, (binding == STB_GLOBAL)
          ? 'T' : 't', keep_ptr[index-1]);*/
        symbol_info.name = &keep_ptr[index-1];
        symbol_info.binding = binding;
        symbol_info.address = st_value;
        symbol_info.size = st_size;
        CVectorAppend(vector_symtab, &symbol_info);
      }
    }
  }
  CVectorSort(vector_symtab, symtab_compare);
  for(int i = 0 ; i < CVectorCount(vector_symtab) ; i++)
  {
      SYMBOL_INFO *symbol_info = (SYMBOL_INFO*) CVectorNth(vector_symtab, i);
      printf("%016lx %016lx %c %s\n", symbol_info->address, symbol_info->size, (symbol_info->binding == STB_GLOBAL)
          ? 'T' : 't', *(const char **)symbol_info->name);
  }
  free(keep_ptr);
  CVectorDispose(vector_symtab);
}

void DisposeElfData(void *data, int size)
{
   munmap(data, size);
}
