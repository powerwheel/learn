/*************************************************************************
	> File Name: fork_test.c
	> Author: 
	> Mail: 
	> Created Time: 2014年07月21日 星期一 16时29分54秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAXLINES 100

static void sig_int(int);
int main()
{
    pid_t pid;
    char buf[MAXLINES];
    int status;

	if(signal(SIGINT,sig_int) == SIG_ERR)
		perror("signal error");
    printf("%% ");
    while(fgets(buf,MAXLINES,stdin) != NULL){
        if(buf[strlen(buf)-1]=='\n')
            buf[strlen(buf)-1] = 0;
         if((pid = fork()) < 0){
             perror(" create Process error");
            exit(0);
         }else if(pid == 0){
             execlp(buf,buf,(char*)0);
             printf("can't exec %s",buf);
            perror("");
            exit(127);
        }
        if((pid = waitpid(pid,&status,0)) < 0){
            perror("wait pid error\n");       
        }
        printf("%% ");
    }
    exit(0);

}
void sig_int(int signo)
{
	printf("interrput\n");
}
