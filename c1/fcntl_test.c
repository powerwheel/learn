#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{
	int val;
	
	if(argc != 2){
		printf("usage: a.out desc\n");
		return 0;
	}
	
	if((val = fcntl(atoi(argv[1]),F_GETFL,0)) < 0){
		//sprintf(stdout,"error to to get %d file status",atoi(argv[1]));
		perror("");
		return 0;
	}
	
	switch(val & O_ACCMODE){
		case O_RDONLY:
			printf("read only");
			break;
		case O_WRONLY:
			printf("write only");
			break;
		case O_RDWR:
			printf("read write");
			break;
		default:
			printf("unknow");
			break;
	}
	if(val & O_APPEND)
		printf(", append");
	if(val & O_NONBLOCK)
		printf(", nonblock");
	if(val & O_SYNC)
		printf(", Sync");
	#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC!=O_SYNC)
		if(val & O_FSYNC)
			printf(", synchronous writes");
	#endif
	
	putchar('\n');
	return 0;
}
