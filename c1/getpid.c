/*************************************************************************
	> File Name: getpid.c
	> Author: 
	> Mail: 
	> Created Time: 2014年07月21日 星期一 16时04分43秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    printf("My Pid is:%d\n",(long)getpid());
    printf("My Parent Pid is:%d\n",(long)getppid());
    exit(0);
}
