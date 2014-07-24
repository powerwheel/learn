/*************************************************************************
	> File Name: read_test1.c
	> Author: 
	> Mail: 
	> Created Time: 2014年07月21日 星期一 15时56分49秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    int c;
    while((c=getc(stdin))!= EOF)
        if(putc(c,stdout)==EOF)
            perror("error");
    if(ferror(stdin))
          perror("read error");

    exit(0);
}
