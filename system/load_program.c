/* load_program.c - load_program */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load_program  -  Dynamically load a program
 *------------------------------------------------------------------------
 */
void*	load_program(
	  char		*path		/* Path to the program 		*/
        )
{
/*
    elf_readxinu();  


    int fd = open(RFILESYS, *path, "or");  //load and open helloworld
    kprintf("%s\n", *path);
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    void *file =(void *)getmem(filesize);
    kprintf("%d\n", file);
    elf_load_file(file);
  
   //return main's address :.text starting point;
    char *mainadd =elf_lookup_main(file);
    kprintf("main function, %d ",mainadd );
	return mainadd;

*/

return NULL;


}
