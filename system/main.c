/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

extern uint32 nsaddr;
//extern Elf32_Ehdr *syshdr ;

funrec funrecs[31]; 

process	main(void)
{
	/* Start the network */

	netstart();
/*
    void* helloworld = load_program("helloworld");
	resume(create(helloworld, 4096, 20, "helloworld", 2, 0, NULL));


		
	int myvalue = 2;
    int j;
    int result = load_library("myadd");
    int result2 = load_library("myadd2");
		// Load the library 
		if(result == SYSERR) {
                 for (j=0; j<30; j++){
      kprintf(" isdirty: %d         ", funrecs[j].isdirty);
      kprintf("name: %s\n", funrecs[j].funcname);
      
     }
  
            kprintf("Error or not? %d",result );
			return SYSERR;
		}

		// Find the add1 function 
		int32 (*add1)(int32) = find_library_function("add10");
		if((int32)add1 == SYSERR) {
			return SYSERR;
		}

     
		// Call the function 
		kprintf("everything done\n");
		kprintf("%d\n", add1(myvalue));
		

     for (j=0; j<30; j++){
      kprintf(" isdirty: %d         ", funrecs[j].isdirty);
      kprintf("name: %s\n", funrecs[j].funcname);
      
     }




   void* hihi = load_program("hihi");
   resume(create(hihi, 4096, 20, "helloworld", 2, 0, NULL));

*/
   









    /*for shell*/

	

	kprintf("\n...creating a shell\n");
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	//* Wait for shell to exit and recreate it 

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}




	
	return OK;
}
