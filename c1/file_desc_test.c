#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(int ragc, char *argv[])
{
	int fd1,fd2,fd3;

	if((fd1 = open("/home/kk/test.c",O_RDONLY)) == -1){
		perror("open error");
		return 0;
	}

	if((fd2 = dup(fd1)) == -1){
		perror("dup error");
		return 0;
	}
	fd3 = open("/home/kk/test.c",O_RDWR);
	if(fd3 == -1){
		perror("open error");
		return 0;
	}

	int fd1_f,fd2_f,fd3_f;
	fd1_f = fcntl(fd1,F_GETFD,0);
	fd2_f = fcntl(fd2,F_GETFD,0);
	fd3_f = fcntl(fd3,F_GETFD,0);

	printf("file desc flag: fd1:%d fd2:%d fd3:%d\n",fd1_f,fd2_f,fd3_f);

	int f1_flag,f2_flag,f3_flag;

	f1_flag = fcntl(fd1,F_GETFL,0);
	f2_flag = fcntl(fd2,F_GETFL,0);
	f3_flag = fcntl(fd3,F_GETFL,0);


	printf("file status flag: fd1:%d fd2:%d fd3:%d\n",f1_flag,f2_flag,f3_flag);


	printf("set fd2\n");
	f2_flag |= O_APPEND;
	if(fcntl(fd2,F_SETFL,f2_flag) == -1){
		perror("set error");
		return 0;
	}

	f1_flag = fcntl(fd1,F_GETFL,0);
	f2_flag = fcntl(fd2,F_GETFL,0);
	f3_flag = fcntl(fd3,F_GETFL,0);

	printf("file status flag: fd1:%d fd2:%d fd3:%d\n",f1_flag,f2_flag,f3_flag);


	printf("set fd3\n");
		f3_flag |= O_APPEND;
	if(fcntl(fd3,F_SETFL,f3_flag) == -1){
			perror("set error");
			return 0;
	}

	f1_flag = fcntl(fd1,F_GETFL,0);
		f2_flag = fcntl(fd2,F_GETFL,0);
		f3_flag = fcntl(fd3,F_GETFL,0);

		printf("file status flag: fd1:%d fd2:%d fd3:%d\n",f1_flag,f2_flag,f3_flag);

	return 0;



}
