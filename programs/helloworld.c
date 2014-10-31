/* helloworld.c - main */

#include <xinu.h>

/*------------------------------------------------------------------------
 * main  -  Main function
 *------------------------------------------------------------------------
 */
int	main (
	int	argc,	/* Number of arguments	*/
	char	*argv[]	/* Arguments array	*/
	)
{
	kprintf("Hello World!\n");
	return 0;
}
