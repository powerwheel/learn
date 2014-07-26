#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main()
{
	umask(0);
	if(creat("file1",RWRWRW) < 0){
		perror("create file1 error");
	}
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if(creat("file2", RWRWRW) < 0){
		perror("create file 2 error");
	}
	printf("create:file1:rwrwrw  file2:rw---\r\n");
	return 0;
}
