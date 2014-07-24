#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	char *buf1[] = {"abcfefghij"};
	char *buf2[] = {"ABCDEFGHIJ"};
	
	int fd;
	if((fd = creat("file.hole",666)) == -1){
		perror("create file error");
		return 0;
	}
	if(write(fd,buf1,10)!=10){
		perror("write error");
		return 0;
	}
	
	if(lseek(fd,12345,SEEK_SET) == -1){
		perror("lseek error");
	}
	
	if(write(fd,buf2,10) != 10){
		perror("write error");
	}
	close(fd);
	return 0;
}
