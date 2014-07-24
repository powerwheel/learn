#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main()
{
	if(lseek(STDIN_FILENO,0,SEEK_CUR) == -1){
		printf("can't seek\n");
	}else{
		printf("seek OK\n");
	}
	return 0;
}
