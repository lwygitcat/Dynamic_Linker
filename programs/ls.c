#include <xinu.h>

/*------------------------------------------------------------------------
 * main  -  Main function
 *------------------------------------------------------------------------
 */

/*

struct rfdirent {
		byte	d_type;
		byte	d_name[256];
	};
*/


/*

#define RF_DIRENT_FILE 1
#define RF_DIRENT_DIR  2
*/
/*
	xsh $ ls mydir
	mydir1/
	mydir2/
	myfile1
	myfile2

*/


int	main (
    int	argc,	/* Number of arguments	*/
	char	*argv[]	/* Arguments array	*/
	)
{
    int32 dirfd;
    //kprintf("argc is %d\n", argc );
    if (argc ==1) 
    dirfd = open(RFILESYS, ".", "ro");
    else dirfd = open(RFILESYS, argv[1], "ro");





  	struct rfdirent rfdentry;
	int32 rc = read(dirfd, (char *)&rfdentry, sizeof(struct rfdirent));

    while (1){
    	if(rc == SYSERR) { /* error accured while reading */ 
          kprintf("ERROR: dir not found \n");
          close(dirfd);
          return SYSERR;
	    } 
	    else if(rc == 0) { /* Reached end of list */
        kprintf("Not DIR: End of file\n");
        close(dirfd);
        return OK;
 	 
        }
	    else { /* Handle next entry in the directory */
           if (rfdentry.d_type==RF_DIRENT_FILE)
           kprintf("%s\n", rfdentry.d_name);
           else kprintf("%s/\n", rfdentry.d_name);
           rc = read(dirfd, (char *)&rfdentry, sizeof(struct rfdirent));
	    }
     }

}


