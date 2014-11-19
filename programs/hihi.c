/* helloworld.c - main */

#include <xinu.h>

/*------------------------------------------------------------------------
 * main  -  Main function
 *------------------------------------------------------------------------
 */


int32	add1 (
	int32	val	/* Value to be increased */
	)
{
	return (val + 1);
}




int a;

int	main (
	int	argc,	/* Number of arguments	*/
	char	*argv[]	/* Arguments array	*/
	)
{
    a=add1(1);
    kprintf("a=%d\n",a);
    int b=a+2;
    kprintf("b=%d\n", b);
	kprintf("Hello World!!Hello World!!\n");
	return 0;
}
