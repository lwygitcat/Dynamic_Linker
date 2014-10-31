/* find_library_function.c - find_library_function */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  find_library_function  -  Locate a dynamically loaded library function
 *------------------------------------------------------------------------
 */
void*	find_library_function(
	  char		*name		/* Function name to locate	*/
        )
{
	return (void*)SYSERR;
}