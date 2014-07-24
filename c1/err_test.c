/*************************************************************************
	> File Name: err_test.c
	> Author: 
	> Mail: 
	> Created Time: 2014年07月21日 星期一 15时05分10秒
 ************************************************************************/

#include<stdio.h>
#include<errno.h>
#include <error.h>
int main()
{
    int i = 0;
    for( i = 0; i < sys_nerr; ++i){
        printf("The Error %d is:%s\n",i,strerror(i));
    }
    return 0;
}
