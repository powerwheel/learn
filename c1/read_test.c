/*************************************************************************
	> File Name: read_test.c
	> Author: 
	> Mail: 
	> Created Time: 2014年07月21日 星期一 15时43分58秒
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>

#define BUFFERSIZE 4096
int main()
{
    char buf[BUFFERSIZE];
    int n;
    while((n = read(STDIN_FILENO,buf,BUFFERSIZE)) > 0)
        if(write(STDOUT_FILENO,buf,n) != n)
            perror("write error;");
    if(n < 0){
        perror("read error");
    }
    return 0;

}
