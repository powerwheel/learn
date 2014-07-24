#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main()
{
	printf("%ld\n",sysconf(_SC_ARG_MAX));
	printf("%ld\n",sysconf(_SC_CHILD_MAX));
	printf("%ld\n",sysconf(_SC_HOST_NAME_MAX));
	return 0;
}
