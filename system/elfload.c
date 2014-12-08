/* elfloader.c- all c file for loading an external elf file */

#include <xinu.h>

//Global variable to store the address of the xinu.elf file. refer to event eventprocess.c

int count=0;
Elf32_Ehdr *syshdr ; //xinu.elf header
int loadmode;
funrec funrecs[31];
static int elf_load_stage1(Elf32_Ehdr *hdr);
static int elf_load_stage2(Elf32_Ehdr *hdr);
int elf_load_stage3(Elf32_Ehdr *hdr);
static int elf_do_reloc(Elf32_Ehdr *hdr, Elf32_Rel *rel, Elf32_Shdr *reltab);
void * elf_lookup_symbol(const char * name);

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
    //return TRUE ;
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
	
	//
	if (loadmode ==2){
		result = elf_load_stage3(hdr);     //put functions in the storage;
		if(result == ELF_RELOC_ERR) {
				kprintf("Unable to load library \n"); 
				return SYSERR;
			} 
		
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
			// TODO : Implement 
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
 *  elf_get_symval  -  Access the symbol address
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
   if(symbol->st_shndx == SHN_UNDEF) { //The Section Related To This Symbol
		// External symbol, lookup value
		Elf32_Shdr *strtab = elf_section(hdr, symtab->sh_link);
		const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;
 
		//extern void *elf_lookup_symbol(const char *name);
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
//kprintf("final target is %d\n", (int)target);
return (int)target;
            
		}

    //segment3 
    } else if(symbol->st_shndx == SHN_ABS) {
		// Absolute symbol
		return symbol->st_value;

	} else if(symbol->st_shndx == SHN_COMMON) {
		if (symbol->st_value > (int)hdr)
            return symbol->st_value;
        else{
            void * mem =getmem(symbol->st_size);
            symbol->st_value = (int)mem;
            return (int)mem;
            }

	}else {
		// Internally defined symbol
		Elf32_Shdr *target = elf_section(hdr, symbol->st_shndx);
       

		return (int)hdr + symbol->st_value + target->sh_offset;
	}
}



/*----------------------------------------------------------------------------------
 *  elf_lookup_symbol  -  Look up symbol using symbol name in xinu.elf's symtab  ( xinu.elf do not have relocation section )
 *----------------------------------------------------------------------------------
 */

void * elf_readxinu()
{
    int fd = open(RFILESYS, "xinu.elf", "or");
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
   // kprintf("xinu.elf size: %d \n", filesize);
    void *c = getmem(filesize);
    int rc = read(fd, c, filesize);
    syshdr =(Elf32_Ehdr *)c;
   // kprintf("readxinu here %d\n", syshdr);
    close(fd);
    return NULL;
}

int strcomp(const char* s1, const char* s2)
{
     while(*s1 && (*s1==*s2))
         s1++,s2++;
     return *(unsigned char*)s1-*(unsigned char*)s2;
  }



void * elf_lookup_symbol(const char * name)   //search symtab, find .text section, target it?
{
   // kprintf("%s to be found in xinu\n", name);
    Elf32_Shdr *sysshdr = elf_sheader(syshdr); //xinu.elf section header


    int i, idx;  //section, symbol names
    Elf32_Shdr *symtab =NULL;
    Elf32_Shdr *sysstrtab =NULL;
   	for(i = 0; i < syshdr->e_shnum; i++) {
		Elf32_Shdr *section = &sysshdr[i];
 
		// Find the symtab section and its strtab, assuming only 1 symtab
		if(section->sh_type == SHT_SYMTAB) {
        //kprintf("find symtab section \n");
        symtab =section;
        sysstrtab = elf_section(syshdr, symtab->sh_link);
        break;}  //Assuming only one symtab
        
        
    }

     
      int symaddr = (int)syshdr + symtab->sh_offset; //symtab real address
      Elf32_Sym *symbol = NULL;
      Elf32_Shdr *target = NULL; //target memory unit 

	  for(idx = 0; idx < symtab->sh_size / symtab->sh_entsize; idx++) {
       // find name 
       symbol = &((Elf32_Sym *)symaddr)[idx];
       int mark = (int)syshdr + sysstrtab->sh_offset + symbol->st_name;
       char *symbolname =(char*)mark;

           if (strcomp(name,symbolname)==0){
          // kprintf("address before: %d\n", idx);
           target = elf_section(syshdr, symbol->st_shndx);
          // kprintf("%s function found in xinu.symtab\n",symbolname);
           break;
        }				
	  }
  if (idx == symtab->sh_size / symtab->sh_entsize){
      kprintf("could not find function \n");
      return NULL;
  }



  return (void*)(symbol->st_value);  // right
}



















void * elf_lookup_main(Elf32_Ehdr *hdr){  //get main address, return main
 

    Elf32_Shdr *shdr = elf_sheader(hdr);

    int i, idx;  //section, symbol names
    Elf32_Shdr *symtab =NULL;
    Elf32_Shdr *strtab =NULL;
   	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 
		// Find the symtab section and its strtab, assuming only 1 symtab
		if(section->sh_type == SHT_SYMTAB) {
        //kprintf("find helloworld symtab section \n");
        symtab =section;
        strtab = elf_section(hdr, symtab->sh_link);
        break;}  //Assuming only one symtab        
    }
  

    int symaddr = (int)hdr + symtab->sh_offset; //symtab real address
      Elf32_Sym *symbol = NULL;
      Elf32_Shdr *target = NULL; //target memory unit 

	  for(idx = 0; idx < symtab->sh_size / symtab->sh_entsize; idx++) {
       // find name 
       symbol = &((Elf32_Sym *)symaddr)[idx];
       int mark = (int)hdr + strtab->sh_offset + symbol->st_name;
       char *symbolname =(char*)mark;



      // if (*name == *symbolname ){
           if (strcomp("main",symbolname)==0){
           target = elf_section(hdr, symbol->st_shndx);//main real address in the file .text
           break;
        }				
	  }
     if (idx == symtab->sh_size / symtab->sh_entsize){
      kprintf("could not find main \n");
      return NULL;
  }

     return (char *)((int)hdr + target->sh_offset+symbol->st_value);
}


/*----------------------------------------------------------------------------------
 *  elf_load_stage1  -  Try Loading the ELF file, and Getmemory for BSS Sections
 *----------------------------------------------------------------------------------
 */

//for uninitialized variables
void memsets(void *mem, int setnumber, int size){
    char *setmem =(char*)mem;
    uint32 i;
    for (i=0; i<size; i++){
    *(setmem+i) =0;
    }
}





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
 //Assuming only one bss section        
				void *mem = getmem(section->sh_size);
                //kprintf("memory here ..................%d......................\n",mem);
				memsets(mem, 0, section->sh_size);
 
				// Assign the memory offset to the section offset
				section->sh_offset = (int)mem - (int)hdr;
				//kprintf("Allocated memory for a section (%ld).\n", section->sh_size);
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
 
	 int i, idx;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
			// Process each entry in the table
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)hdr + section->sh_offset))[idx];
				int result = elf_do_reloc(hdr, reltab, section);
				if(result == ELF_RELOC_ERR) {
					kprintf("Failed to relocate symbol.\n");
					return ELF_RELOC_ERR;
				}
			}
		}
	}
	return 0;
}








/*----------------------------------------------------------------------------------
 *  putfunc  -  Load library, put the sym to the function info
 *----------------------------------------------------------------------------------
 */

int putfunc(char *funcname, int funcaddr)
{
	 //prepare some slot for unload library
	int id;
	
	
	//check numduplicate, check if dirty
	for (id =1; id<=30; id++){
		if((strcomp(funcname, funrecs[id].funcname)==0)	  &&   (funrecs[id].isdirty ==1) ){ 
        //  if(id ==0) {
         //    kprintf("sorry, two main not allowed !\n");
         //    return -1;
          // }
		 // else {	
             kprintf("gotta! , you are already here, id number: %d,   name : %s\n", id, funrecs[id].funcname);
			 return -1;
          // }
          
		}
		
	}
	
	for (id =1; id<=30; id++){
		//waht's the initial value of struct
		if(funrecs[id].isdirty ==0){
			funrecs[id].funcname = funcname;
			funrecs[id].funcaddr =funcaddr;
			funrecs[id].isdirty =1;
			break;
		}		
		
	}
	if (id>30)
		kprintf("you have already loaded 3 libraries, wait for unload\n");
	return 0;
	
}




/*----------------------------------------------------------------------------------
 *  elf_load_stage3  -  Try To Load all the functions to the set
 *----------------------------------------------------------------------------------
 */

 int elf_load_stage3(Elf32_Ehdr *hdr) {
	Elf32_Shdr *shdr = elf_sheader(hdr);
 
	 int i;
     int idx =0;
     int idy=0;
	 int funcount=0;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
      
		// If this is a symtab section, more than 1 symtab is OK
		if(section->sh_type == SHT_SYMTAB) {
 
           Elf32_Shdr *strtab = elf_section(hdr, section->sh_link);


			//check if too many functions in a library;
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
                Elf32_Sym *tempsym = &((Elf32_Sym *)((int)hdr + section->sh_offset))[idx];
                
				if (    (ELF32_ST_BIND(tempsym->st_info)==STB_GLOBAL)  &&  (ELF32_ST_TYPE(tempsym->st_info)==STT_FUNC)   ){
					funcount++;
                  int mark = (int)hdr + strtab->sh_offset + tempsym->st_name;
				  char *funcname =(char*)mark;	
                  if (strcomp(funcname, "main")==0)	 {
                  kprintf("sorry, two main not allowed !\n");
                  return -1;
                 }
                   
              
                  }
           //     kprintf("funcount now is %d\n", funcount);
				if(funcount >10){
				   kprintf("trying to load too many functions \n");
				   return SYSERR;
				}
				   
			}
			
			
			
			
			
			//funnum appropriate, Load functions
     // Elf32_Shdr *strtab = elf_section(hdr, section->sh_link);
			for(idy = 0; idy < section->sh_size / section->sh_entsize; idy++) {
				
				Elf32_Sym *tempsym = &((Elf32_Sym *)((int)hdr + section->sh_offset))[idy];
				/*if Function, Bind is Global, a function to be put into */
				if (    (ELF32_ST_BIND(tempsym->st_info)==STB_GLOBAL)  &&  (ELF32_ST_TYPE(tempsym->st_info)==STT_FUNC)   ){
				       int mark = (int)hdr + strtab->sh_offset + tempsym->st_name;
				       char *funcname =(char*)mark;		
				      Elf32_Shdr *target = elf_section(hdr, tempsym->st_shndx); //func in the map
				       int funcaddress = (int)hdr + target->sh_offset+tempsym->st_value;
					   int result = putfunc(funcname,funcaddress);
				       
				        if (result == ELF_RELOC_ERR){
				        	kprintf("Conflict function or library \n");
				        	return SYSERR;
				        }
					
				}
				

				
				
				}
		}
		
			
		
	}
	return 0;
}




/*----------------------------------------------------------------------------------
 *  elf_do_reloc  -  Relocate the symbols
 *----------------------------------------------------------------------------------
 */
static int elf_do_reloc(Elf32_Ehdr *hdr, Elf32_Rel *rel, Elf32_Shdr *reltab) {
    //kprintf("#%d to be relocated\n", count++);

	Elf32_Shdr *target = elf_section(hdr, reltab->sh_info);
 
	int addr = (int)hdr + target->sh_offset;
	int *ref = (int *)(addr + rel->r_offset);




   // Symbol value

	int symval = 0;

	if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
		symval = elf_get_symval(hdr, reltab->sh_link, ELF32_R_SYM(rel->r_info));
		if(symval == ELF_RELOC_ERR) return ELF_RELOC_ERR;
	}

   
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
   

//set load mode, can either be 1: load_program or 2: load_library
   int setloadmode(int mode){
	   loadmode = mode;
       return 0;
	   
   }

