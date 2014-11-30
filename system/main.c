/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

extern uint32 nsaddr;
//extern Elf32_Ehdr *syshdr ;

process	main(void)
{
	/* Start the network */

	netstart();

    void* helloworld = load_program("helloworld");
    void* hihi = load_program("hihi");
    void* helloworld2 = load_program("helloworld");
	resume(create(helloworld, 4096, 20, "helloworld", 2, 0, NULL));
	resume(create(hihi, 4096, 20, "helloworld", 2, 0, NULL));
    resume(create(helloworld2, 4096, 20, "helloworld2", 2, 0, NULL));
   

	return OK;
}
