/* helloworld.c - main */

#include <xinu.h>

/*------------------------------------------------------------------------
 * main  -  Main function
 *------------------------------------------------------------------------
 */

/*
xsh $ semdump
	Entry	State	Count	Queue
	0	S_USED	3	3
	1	S_FREE	0	0
*/

/*
struct	sentry	{
	byte	sstate;		/* Whether entry is S_FREE or S_USED	
	int32	scount;		/* Count for the semaphore		
	qid16	squeue;		/* Queue of processes that are waiting	
					
};

*/


//struct	sentry semtab[];

int	main (
	int	argc,	/* Number of arguments	*/
	char	*argv[]	/* Arguments array	*/
	)
{
     //sid32 semid = semcreate(1);
     //sid32 semid2 = semcreate(3);
     
    kprintf("	Entry	State	Count	Queue\n");
    int i; 
    char* type[2]={"S_USED","S_FREE"};
    for (i=0; i<NSEM; i++){
    if (semtab[i].sstate== S_USED)
     kprintf("	%d	%s	%d	%d\n", i,type[0],semtab[i].scount,semtab[i].squeue);
    else
      kprintf("	%d	%s	%d	%d\n", i,type[1],semtab[i].scount,semtab[i].squeue);

    }

	return OK;
}


