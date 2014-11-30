

/*Message printing when something wrong*/

# define WRITE(PRE, STR, ...)	\
	kprintf("[%s] %s() : ", (PRE), (__FUNCTION__));	\
	kprintf(STR, __VA_ARGS__);
 
# define DEBUG(STR, ...)	\
	WRITE("INFO", STR, __VA_ARGS__);
 
# define WARN(STR, ...)		\
	WRITE("WARN", STR, __VA_ARGS__);
 
# define ERROR(STR, ...)	\
	WRITE("ERR!", STR, __VA_ARGS__);


// define all the ELF datatypes
typedef uint16 Elf32_Half;	// Unsigned half int
typedef uint32 Elf32_Off;	// Unsigned offset
typedef uint32 Elf32_Addr;	// Unsigned address
typedef uint32 Elf32_Word;	// Unsigned int
typedef int32  Elf32_Sword;	// Signed int


/*ELF header information*/

# define ELF_NIDENT	16
// #pragma pack(1)  Force no padding 
#pragma pack(1)
typedef struct {
	uint8		e_ident[ELF_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr;
#pragma pack() 


/*elf identification*/
enum Elf_Ident {
	EI_MAG0		= 0, // 0x7F
	EI_MAG1		= 1, // 'E'
	EI_MAG2		= 2, // 'L'
	EI_MAG3		= 3, // 'F'
	EI_CLASS	= 4, // Architecture (32/64)
	EI_DATA		= 5, // Byte Order
	EI_VERSION	= 6, // ELF Version
	EI_OSABI	= 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		= 9  // Padding
};
 
# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(1)  // 32-bit Architecture



/*if relocation, not exec file check  "NOT NECESSARY?"*/
// If file begins with "ELF" and if File can be loaded into the target system
enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};
 
# define EM_386		(3)  // x86 Machine Type
# define EV_CURRENT	(1)  // ELF Current Version

# define ELF_RELOC_ERR -1  // ELF-RELOC Error Anytime During The Process

/*Section Header struct*/
#pragma pack(1)
typedef struct {
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} Elf32_Shdr;
#pragma pack() 


/*Section Header Types and Attributes field*/
# define SHN_UNDEF	(0x00) // Undefined/Not present
# define SHN_ABS     (0xfff1) //absolute value, not change because of relocation
# define SHN_COMMON   (0xfff2) //Uninitialized variables


enum ShT_Types {
	SHT_NULL	= 0,   // Null section
	SHT_PROGBITS	= 1,   // Program information
	SHT_SYMTAB	= 2,   // Symbol table
	SHT_STRTAB	= 3,   // String table
	SHT_RELA	= 4,   // Relocation (w/ addend)
	SHT_NOBITS	= 8,   // Not present in file
	SHT_REL		= 9,   // Relocation (no addend)
};
 
enum ShT_Attributes {
	SHF_WRITE	= 0x01, // Writable section
	SHF_ALLOC	= 0x02  // Exists in memory
};



/*Symbols in Symtab*/
#pragma pack(1)
typedef struct {
	Elf32_Word		st_name;
	Elf32_Addr		st_value;
	Elf32_Word		st_size;
	uint8   		st_info;
	uint8			st_other;
	Elf32_Half		st_shndx;
} Elf32_Sym;
#pragma pack()


/*Symbol Attributes and Bindings*/
# define ELF32_ST_BIND(INFO)	((INFO) >> 4)
# define ELF32_ST_TYPE(INFO)	((INFO) & 0x0F)
 

enum StT_Bindings {
	STB_LOCAL		= 0, // Local scope
	STB_GLOBAL		= 1, // Global scope
	STB_WEAK		= 2  // Weak, (ie. __attribute__((weak)))
};
 
enum StT_Types {
	STT_NOTYPE		= 0, // No type
	STT_OBJECT		= 1, // Variables, arrays, etc.
	STT_FUNC		= 2  // Methods or functions
};


/*REL and RELA Section Entry information, 4 bytes each, always match*/
#pragma pack(1)
typedef struct {
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
} Elf32_Rel;
#pragma pack()


#pragma pack(1) 
typedef struct {
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
	Elf32_Sword		r_addend;
} Elf32_Rela;
#pragma pack()


/*Relocation Type for Non-dynamic Linking*/
# define ELF32_R_SYM(INFO)	((INFO) >> 8)
# define ELF32_R_TYPE(INFO)	((uint8)(INFO))
 
enum RtT_Types {
	R_386_NONE		= 0, // No relocation
	R_386_32		= 1, // Symbol + Offset
	R_386_PC32		= 2  // Symbol + Offset - Section Offset
};

/*Relocating Rules*/
# define DO_386_32(S, A)	((S) + (A))
# define DO_386_PC32(S, A, P)	((S) + (A) - (P))







/*put in the very end, xinu.elf address public */
extern	Elf32_Ehdr *syshdr;
// Used in other .c file format: extern pid32 eventprocesspid;
