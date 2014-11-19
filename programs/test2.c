/* helloworld.c - main */

#include <xinu.h>

/*------------------------------------------------------------------------
 * main  -  Main function
 *------------------------------------------------------------------------
 */
int a=1, b=2,d=5;
char hi;
int	main (
	int	argc,	/* Number of arguments	*/
	char	*argv[]	/* Arguments array	*/
	)
{
    int c=3;
	kprintf("Hello World!\n");
    say("hi");
    c=a+b;
    d=0;
    hi ="yellow";
	return 0;
}
