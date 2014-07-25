#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int i;
	struct stat buf;
	char *ptr;

	for(i = 1; i < argc; ++i){
		if(stat(argv[i],&buf) == -1){
			printf("stat error on:%s",argv[i]);
			perror("");
			return 0;
		}
		if(S_ISREG(buf.st_mode))
			ptr = "regluar file";
		else if(S_ISCHR(buf.st_mode))
			ptr = "Char file";
		else if(S_ISBLK(buf.st_mode))
			ptr = "block file";
		else if(S_ISDIR(buf.st_mode))
			ptr = "Directory";
		else if(S_ISFIFO(buf.st_mode))
			ptr = "FILO";
		else if(S_ISSOCK(buf.st_mode))
			ptr = "Socket";
		else if(S_ISLNK(buf.st_mode))
			ptr = "Link File";
		else
			ptr = "Unknow File Type";
		printf("%s\n",ptr);
	}
	return 0;
}
