#include <dirent.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;
    if(argc != 2){
        perror("uage: ls dir name");
        exit(1);
    }
    if((dp = opendir(argv[1]))== NULL){
       // printf("%s\n",strerror(errno));
        //
        perror("Can;t open:");
        exit(1);
    }
    while((dirp = readdir(dp)) != NULL){
        printf("%s\n",dirp->d_name);
    }
    return 0;
}
