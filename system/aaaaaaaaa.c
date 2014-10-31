//for future use in load_program
void *elf_load_file(char* file) {  //file is the file name, in this case helloworld, well done char*file
   
    /*file address in memory*/
    char *sysfile = NULL;
    char *targetfile = NULL;  
    

    //get memory for target file
    int fd = open(RFILESYS, file, "or");
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    targetfile = getmem (filesize);

    //get memory for xinu.elf
    int sysfd = open(RFILESYS, "xinu.elf", "or");
    int32 xinusize = control(RFILESYS, RFS_CTL_SIZE, sysfd, 0);
    sysfile = getmem (xinusize);

    
    // get the Elf header for this two files
    Elf32_Ehdr *targethdr = (Elf32_Ehdr *)targetfile;
    Elf32_Ehdr *syshdr = (Elf32_Ehdr *)file;

    
    /*Ignore type check, assume this two files are -----maybe need to be added back later*/
/*    
    if(!elf_check_supported(hdr)) {
		ERROR("ELF File cannot be loaded.\n");
		return;
	}

	switch(tarhdr->e_type) {
		case ET_EXEC:
			// TODO : Implement
			return NULL;
		case ET_REL:
			return elf_load_rel(hdr);
	}
	return NULL;
*/
    return targethdr; 
}
