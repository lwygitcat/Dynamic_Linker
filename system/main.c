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


/*
    int fd = open(RFILESYS, "xinu.elf", "or");
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    kprintf("size: %d", filesize); //size of xinu.elf
    char *c =getmem(filesize);
    syshdr = (Elf32_Ehdr *)c; 
    int rs =read(fd,c,filesize);
*/






    int fd = open(RFILESYS, "hihi", "or");  //load and open helloworld
    //kprintf("%s\n", *path);
    int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
    void *file =(void *)getmem(filesize);
    int rc = read(fd, file, filesize);
    kprintf("read helloworld here %d\n", file);

    elf_readxinu();  /*loading xinu.elf into memory*/
    elf_load_file(file);




  
   //return main's address :.text starting point;
    char *mainadd =elf_lookup_main(file);
    kprintf("main function, %d ",mainadd );
	//return mainadd;






   // void* helloworld = load_program("helloworld");
	resume(create(mainadd, 4096, 20, "helloworld", 2, 0, NULL));










	return OK;
}
