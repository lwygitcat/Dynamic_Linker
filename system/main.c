/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

extern uint32 nsaddr;
//extern Elf32_Ehdr *syshdr ;

process	main(void)
{
	/* Start the network */

	netstart();

	//kprintf("\n...creating a shell\n");
//	recvclr();
//	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

/*	while (TRUE) {
		receive();  //when shell killed, sent a msg to parent guessing
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}*/

    int fd = open(RFILESYS, "xinu.elf", "or");
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    kprintf("size: %d", filesize); //size of helloworld
    char *c =getmem(filesize);
    syshdr = (Elf32_Ehdr *)c; 
    int rs =read(fd,c,filesize);
	return OK;
}
