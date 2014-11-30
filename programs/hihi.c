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




int a=2;
int c;
int d;
int e;
char ye;
char xi;

int	main (
	int	argc,	/* Number of arguments	*/
	char	*argv[]	/* Arguments array	*/
	)
{
    a=add1(1);
    c=2;
    d=3;
    e=10;
    ye='l';
    xi ='x';
    kprintf("a=%d\n",a);
    int b=a+2;
    kprintf("b=%d\n", b);
    kprintf("c=%d\n", c);
    kprintf("d=%d\n",d);
    kprintf("e=%d\n",e);
    kprintf("ye = %c\n", ye);
    kprintf("xi = %c\n", xi);
	kprintf("Hello World!!Hello World!!\n");
	return 0;
}
