/* find_library_function.c - find_library_function */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  find_library_function  -  Locate a dynamically loaded library function
 *------------------------------------------------------------------------
 */

funrec funrecs[31];

void*	find_library_function(
	  char		*name		/* Function name to locate	*/
        )
{
   int id;
	//void *addr;
	for (id =1; id<=30; id++){
		if((strcomp(name, funrecs[id].funcname)==0)	  &&   (funrecs[id].isdirty ==1) ){
			kprintf("find_library_function success\n");
			return (void *) funrecs[id].funcaddr;
		}
		
	}
	
	//function not found
	kprintf("function not found in library\n");
	return (void*)SYSERR;
}







