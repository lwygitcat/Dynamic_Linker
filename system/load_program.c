/* load_program.c - load_program */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load_program  -  Dynamically load a program
 *------------------------------------------------------------------------
 */
static int xinuload_count=0;
void*	load_program(
	  char		*path		/* Path to the program 		*/
        )
{
   setloadmode(1); //load functions
   
   
   if(xinuload_count==0){
    xinuload_count++;
    elf_readxinu(); 
  }
  

    int fd = open(RFILESYS, path, "or");  //load and open helloworld
    if (fd == -1)
    return SYSERR;
   // kprintf("%s\n", *path);
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    void *file =(void *)getmem(filesize);
    int rc = read(fd, file, filesize);
    close(fd);

    
    int result = elf_load_file(file);  
    if (result == SYSERR)
     return SYSERR;
    char *mainadd =(char *)elf_lookup_main(file);
   // kprintf("main function, %d ",mainadd );
	

return mainadd;


}
