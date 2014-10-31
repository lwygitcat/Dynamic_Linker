/* elfloader.c- all c file for loading an external elf file */

#include <xinu.h>

/* -----------------------------------------------------------------------
* To be solved
* 1. All Error message in the elf_load_file not defined
* 2. Function elf_load_stage1(),elf_load_stage2() not defined >DECIDED to delete the elf_load_rel function.
* 3. in elf_load_file(), does xinu support string type? > using char[] type
* 4. How to decide where the file is loaded > using a char[] big enough to hold it.
* 5. We need to make the memory address of the xinu.elf public
* 6. Ignoring section names functions, Is it necessary?
* 7. [!! Important]Error, Write, Debug, Warn functions inside need to be modifed
* 9. [!! Important]elf_lookup_symbol(name) // find the symbol address based on the symbol name need to be modified
* 10. Alternative way of implementing memset, make all the bss section 0
* 11. Double check the reloc.h file, may missing something or duplicate
*-------------------------------------------------------------------------
*/


//Global variable to store the address of the xinu.elf file. refer to event eventprocess.c
Elf32_Ehdr *syshdr ;
static int elf_load_stage1(Elf32_Ehdr *hdr);
static int elf_load_stage2(Elf32_Ehdr *hdr);
static int elf_do_reloc(Elf32_Ehdr *hdr, Elf32_Rel *rel, Elf32_Shdr *reltab);

/*Check the starting bits of the elf file*/
bool8 elf_check_file(Elf32_Ehdr *hdr) {
	if(!hdr) return SYSERR;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		kprintf("ELF Header EI_MAG0 incorrect.\n");
		return SYSERR;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		kprintf("ELF Header EI_MAG1 incorrect.\n");
		return SYSERR;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		kprintf("ELF Header EI_MAG2 incorrect.\n");
		return SYSERR;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		kprintf("ELF Header EI_MAG3 incorrect.\n");
		return SYSERR;
	}
	return TRUE;
}


/*check other ELF file header matches requirements*/
bool8 elf_check_supported(Elf32_Ehdr *hdr) {
	if(!elf_check_file(hdr)) {
		kprintf("Invalid ELF File.\n");
		return SYSERR;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		kprintf("Unsupported ELF File Class.\n");
		return SYSERR;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		kprintf("Unsupported ELF File byte order.\n");
		return SYSERR;
	}
	if(hdr->e_machine != EM_386) {
		kprintf("Unsupported ELF File target.\n");
		return SYSERR;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		kprintf("Unsupported ELF File version.\n");
		return SYSERR;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		kprintf("Unsupported ELF File type.\n");
		return SYSERR;
	}
	return TRUE;
}




/*------------------------------------------------------------------------
 *  elf_load_file  -  Loading the ELF file
 *  Pass filename as parameter, using load to read from rfssystem
 *------------------------------------------------------------------------
 */

/*Load the ELF file*/
static inline void *elf_load_rel(Elf32_Ehdr *hdr) {
	int result;
	result = elf_load_stage1(hdr);
	if(result == ELF_RELOC_ERR) {
		kprintf("Unable to load ELF file.\n");
		return NULL;
	} 
	result = elf_load_stage2(hdr);
	if(result == ELF_RELOC_ERR) {
		kprintf("Unable to load ELF file.\n");
		return NULL;
	}
	// TODO : Parse the program header (if present)
	return (void *)hdr->e_entry;  
}


void *elf_load_file(void *file) {
	Elf32_Ehdr *hdr = (Elf32_Ehdr *)file;
	if(!elf_check_supported(hdr)) {
		kprintf("ELF File cannot be loaded.\n");
		return NULL;
	}
	switch(hdr->e_type) {
		case ET_EXEC:
			// TODO : Implement -----------------------------------------For Helloworld, will it cause something wrong here?
			return NULL;
		case ET_REL:
			return elf_load_rel(hdr);
	}
	return NULL;
}






/*----------------------------------------------------------------------------------
 *  elf_section  -  Accessing Section Header and All the Section Entry in the Header
 *  Pass filename as parameter, using load to read from rfssystem
 *----------------------------------------------------------------------------------
 */
static inline Elf32_Shdr *elf_sheader(Elf32_Ehdr *hdr) {
	return (Elf32_Shdr *)((int)hdr + hdr->e_shoff);
}
 
static inline Elf32_Shdr *elf_section(Elf32_Ehdr *hdr, int idx) {
	return &elf_sheader(hdr)[idx];
}





/*----------------------------------------------------------------------------------
 *  elf_get_symval  -  Access the symbol address [still some confusions here!]
 *----------------------------------------------------------------------------------
 */
static int elf_get_symval(Elf32_Ehdr *hdr, int table, uint32 idx) {
	if(table == SHN_UNDEF || idx == SHN_UNDEF) return 0;
	Elf32_Shdr *symtab = elf_section(hdr, table);
 
	if(idx >= symtab->sh_size) {
		kprintf("Symbol Index out of Range (%d:%u).\n", table, idx);
		return ELF_RELOC_ERR;
	}
 
	int symaddr = (int)hdr + symtab->sh_offset;
	Elf32_Sym *symbol = &((Elf32_Sym *)symaddr)[idx];
   
   //segment2
   if(symbol->st_shndx == SHN_UNDEF) { //The Section Related To This Symbol---------------------------------------------HERE NEED BIG MODIFICATION
		// External symbol, lookup value
		Elf32_Shdr *strtab = elf_section(hdr, symtab->sh_link);
		const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;
 
		extern void *elf_lookup_symbol(const char *name);
		void *target = elf_lookup_symbol(name);
 
		if(target == NULL) {
			// Extern symbol not found
			if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK) {
				// Weak symbol initialized as 0
				return 0;
			} else {
				kprintf("Undefined External Symbol : %s.\n", name);
				return ELF_RELOC_ERR;
			}
		} else {
			return (int)target;
		}

    //segment3 
    } else if(symbol->st_shndx == SHN_ABS) {
		// Absolute symbol
		return symbol->st_value;
	} else {
		// Internally defined symbol
		Elf32_Shdr *target = elf_section(hdr, symbol->st_shndx);
		return (int)hdr + symbol->st_value + target->sh_offset;
	}
}



/*----------------------------------------------------------------------------------
 *  elf_lookup_symbol  -  Look up symbol using symbol name in xinu.elf's symtab  ( xinu.elf do not have relocation section )
 *----------------------------------------------------------------------------------
 */
/* Xinu.elf  Section Header Inforamtion
Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        00100000 001000 014000 00  AX  0   0 32
  [ 2] .eh_frame         PROGBITS        00114000 015000 00325c 00   A  0   0  4
  [ 3] .data             PROGBITS        00117260 018260 001da0 00  WA  0   0 32
  [ 4] .bss              NOBITS          00119000 01a000 020000 00  WA  0   0 32
  [ 5] .comment          PROGBITS        00000000 01a000 00002d 01  MS  0   0  1
  [ 6] .shstrtab         STRTAB          00000000 01a02d 00003f 00      0   0  1
  [ 7] .symtab           SYMTAB          00000000 01a1d4 002e20 10      8 303  4
  [ 8] .strtab           STRTAB          00000000 01cff4 0019af 00      0   0  1

*/

/* ========================need to be added 
void * elf_lookup_symbol(char * name)
{
  //iterate through the symtable of xinu.elf----[ Section type = SHT_SYMTAB =2]
  //if its name matched the name of the symbol, break; 
  // fetch infomation of the symbol for lookup_symbol

}
*/



/*----------------------------------------------------------------------------------
 *  elf_load_stage1  -  Try Loading the ELF file, and Getmemory for BSS Sections
 *----------------------------------------------------------------------------------
 */

/* Section Header info of helloworld
Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        00000000 000034 00001c 00  AX  0   0  4
  [ 2] .rel.text         REL             00000000 0003f8 000010 08     11   1  4
  [ 3] .data             PROGBITS        00000000 000050 000000 00  WA  0   0  4
  [ 4] .bss              NOBITS          00000000 000050 000000 00  WA  0   0  4
  [ 5] .rodata           PROGBITS        00000000 000050 00000e 00   A  0   0  1
  [ 6] .comment          PROGBITS        00000000 00005e 00002e 01  MS  0   0  1
  [ 7] .note.GNU-stack   PROGBITS        00000000 00008c 000000 00      0   0  1
  [ 8] .eh_frame         PROGBITS        00000000 00008c 000038 00   A  0   0  4
  [ 9] .rel.eh_frame     REL             00000000 000408 000008 08     11   8  4
  [10] .shstrtab         STRTAB          00000000 0000c4 00005f 00      0   0  1
  [11] .symtab           SYMTAB          00000000 00032c 0000b0 10     12   9  4
  [12] .strtab           STRTAB          00000000 0003dc 00001b 00      0   0  1

*/
static int elf_load_stage1(Elf32_Ehdr *hdr) {
	Elf32_Shdr *shdr = elf_sheader(hdr);
 
	unsigned int i;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 
		// If the section isn't present in the file
		if(section->sh_type == SHT_NOBITS) {
			// Skip if it the section is empty
			if(!section->sh_size) continue;
			// If the section should appear in memory
			if(section->sh_flags & SHF_ALLOC) {
				// Allocate and zero some memory
				void *mem = getmem(section->sh_size);
				memset(mem, 0, section->sh_size);
 
				// Assign the memory offset to the section offset
				section->sh_offset = (int)mem - (int)hdr;
				kprintf("Allocated memory for a section (%ld).\n", section->sh_size);
			}
		}
	}
	return 0;
}


/*----------------------------------------------------------------------------------
 *  elf_load_stage2  -  Try To Relocate All the Relocation Sections
 *----------------------------------------------------------------------------------
 */
// ELF_RELOC_ERR =-1, defiend in the reloc.h already
static int elf_load_stage2(Elf32_Ehdr *hdr) {
	Elf32_Shdr *shdr = elf_sheader(hdr);
 
	uint32 i, idx;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
			// Process each entry in the table
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)hdr + section->sh_offset))[idx];
				int result = elf_do_reloc(hdr, reltab, section);
				// On kprintf, display a message and return
				if(result == ELF_RELOC_ERR) {
					kprintf("Failed to relocate symbol.\n");
					return ELF_RELOC_ERR;
				}
			}
		}
	}




/*----------------------------------------------------------------------------------
 *  elf_do_reloc  -  Relocate the symbols
 *----------------------------------------------------------------------------------
 */
static int elf_do_reloc(Elf32_Ehdr *hdr, Elf32_Rel *rel, Elf32_Shdr *reltab) {
	Elf32_Shdr *target = elf_section(hdr, reltab->sh_info);
 
	int addr = (int)hdr + target->sh_offset;
	int *ref = (int *)(addr + rel->r_offset);


   //seg2
   // Symbol value
	int symval = 0;
	if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
		symval = elf_get_symval(hdr, reltab->sh_link, ELF32_R_SYM(rel->r_info));
		if(symval == ELF_RELOC_ERR) return ELF_RELOC_ERR;
	}

   //seg3
   // Relocate based on type
	switch(ELF32_R_TYPE(rel->r_info)) {
		case R_386_NONE:
			// No relocation
			break;
		case R_386_32:
			// Symbol + Offset
			*ref = DO_386_32(symval, *ref);
			break;
		case R_386_PC32:
			// Symbol + Offset - Section Offset
			*ref = DO_386_PC32(symval, *ref, (int)ref);
			break;
		default:
			// Relocation type not supported, display kprintf and return
			kprintf("Unsupported Relocation Type (%d).\n", ELF32_R_TYPE(rel->r_info));
			return ELF_RELOC_ERR;
	}
	return symval;
}
   

