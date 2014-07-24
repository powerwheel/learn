#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	fprintf(stderr,"EACCS: %s\n",strerror(EACCES));
	errno = ENOENT;
	perror(argv[0]);
	return 0;
}
