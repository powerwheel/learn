#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main()
{
	#ifdef PATH_MAX
		printf("%d\n",PATH_MAX);
	#endif
	
	#ifdef FOPEN_MAX
		printf("%d\n",FOPEN_MAX);
	#endif
	printf("/ :%ld\n",pathconf("/",_PC_PATH_MAX));
	printf("/usr/share/ :%ld\n",pathconf("/usr/share/",_PC_NAME_MAX));
	return 0;
}
