#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	printf("real user id is: %d\n",(int)getuid());
	printf("calling uid is %d\n",(int)geteuid());
	
		printf("real group id is: %d\n",(int)getgid());
	printf("calling group is %d\n",(int)getegid());
	return 0;
}
