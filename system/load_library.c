/* load_library.c - load_library */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load_library  -  Dynamically load a library
 *------------------------------------------------------------------------
 */


//loadlibrary and load_program will do it 2 times
static int xinuload_count;
funrec funrecs[31];  //store all the function records
libheader libheads[3];// store all the library

static int librarycount;

syscall	load_library(
	  char		*path		/* Path to the library 		*/
        )
{
	librarycount++;
    if (librarycount>3){
      kprintf("Error: Load more than 3 libs\n");
      return SYSERR; 
    }
	setloadmode(2);
    int result;

//check for two main functions with a ghost main
//   funrec maincheck = {"main", 0, 1};
//   funrecs[0] = maincheck;




/*checking xinu.elf loaded*/
  if(xinuload_count==0){
     xinuload_count++;
     elf_readxinu(); 
  }
  


 int fd = open(RFILESYS, path, "or");  //load and open helloworld
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    void *file =(void *)getmem(filesize);
    int rc = read(fd, file, filesize);
    close(fd);
  
//check if syserr
    
     
     result=elf_load_file(file);
     if (result == -1)
     return SYSERR;
     return OK;


}
