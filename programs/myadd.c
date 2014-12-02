/* myadd.c - add1, add2 */

#include <xinu.h>

int32 ett;


/*------------------------------------------------------------------------
 * add1  -  Adds one (1) to the argument and returns result
 *------------------------------------------------------------------------
 */
int32	add1 (
	int32	val	/* Value to be increased */
	)
{
    ett =2;
    kprintf("%d", ett);
	return (val + 1);
}

/*------------------------------------------------------------------------
 * add2  -  Adds two (2) to the argument and returns result
 *------------------------------------------------------------------------
 */
int32	add2 (
	int32	val	/* Value to be increased */
	)
{
	return (val + 2);
}

int32	add3 (
	int32	val	/* Value to be increased */
	)
{
	return (val + 2);
}




int32	add4 (
	int32	val	/* Value to be increased */
	)
{
	return (val + 2);
}



int32	add5 (
	int32	val	/* Value to be increased */
	)
{
	return (val + 2);
}




int32	add6 (
	int32	val	/* Value to be increased */
	)
{
	return (val + 2);
}

