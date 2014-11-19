#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>

#define BUFFER_SIZE 1024

enum CHECK_STATE {CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER};
enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };
enum HTTP_CODE { NO_REAUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION};

static const char* szret[] = {" i get a correct result\n", "Something wrong\n"};

LINE_STATUS parse_line(char *buffer, int &check_index, int &read_index)
{
    char temp;

    for(; check_index < read_index; ++check_index){
        temp = buffer[check_index];

        if(temp == '\r'){
            if((check_index + 1) == read_index){
                return LINE_OPEN;
            }else if(buffer[check_index+1] == '\n'){
                buffer[check_index++] = '\0';
                buffer[check_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }else if(temp == '\n'){
            if((check_index > 1) && (buffer[check_index - 1] == '\r') ){
                buffer[check_index - 1] = '\0';
                buffer[check_index++] = '\0';
                return LINE_OK;
            }
        }
       // return LINE_BAD;
    }
    return LINE_OPEN;
}

HTTP_CODE parse_requestline(char *temp, CHECK_STATE &checkstate)
{
    char *url = strpbrk(temp," \t");
    if(!url){
        return BAD_REQUEST;
    }
    *url++ = '\0';

    char *method = temp;

    if(strcasecmp(method, "GET") == 0){
        printf("the request is GET\r\n");
    }else{
        return BAD_REQUEST;
    }
    url += strspn(url, " \t");
    char *version = strpbrk(url, " \t");
    if(!version){
        return BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn(version, " \t");
    if(strcasecmp(version, "HTTP/1.1") != 0){
        return BAD_REQUEST;
    }
    if(strncasecmp(url, "http://", 7) == 0){
        url += 7;
        url = strchr(url, '/');
    }
    if(!url || url[0] != '/'){
        return BAD_REQUEST;
    }
    printf("the request url is %s\r\n",url);

    checkstate = CHECK_STATE_HEADER;
    return NO_REAUEST;
}

HTTP_CODE parse_header(char *temp)
{
    if(temp[0] == '\0'){
        return GET_REQUEST;
    }
    else if(strncasecmp(temp, "Host:",5) == 0){
        temp += 5;
        temp += strspn(temp, " \t");
        printf("the request host is: %s\n",temp);
    }else{
        printf("i canot handle this header\r\n");
    }
    return NO_REAUEST;
}

HTTP_CODE parse_content(char *buffer, int &check_index, CHECK_STATE &checkstate, int &read_index, int &start_line)
{
    LINE_STATUS linestatus = LINE_OK;
    HTTP_CODE retcode = NO_REAUEST;

    while((linestatus = parse_line(buffer, check_index, read_index)) == LINE_OK){
        char *temp = buffer + start_line;
        start_line = check_index;

        switch(checkstate){
            case CHECK_STATE_REQUESTLINE:
            {
                retcode = parse_requestline(temp, checkstate);
                if(retcode == BAD_REQUEST){
                    return BAD_REQUEST;
                }
                break;
            }
            case  CHECK_STATE_HEADER:
            {
                retcode = parse_header(temp);
                if(retcode == BAD_REQUEST){
                    return BAD_REQUEST;
                }else if(retcode == GET_REQUEST){
                    return GET_REQUEST;
                }
                break;
            }
            default:
            {
                return INTERNAL_ERROR;
            }
        }
    }
    if(linestatus == LINE_OPEN){
        return NO_REAUEST;
    }else{
        return BAD_REQUEST;
    }
}

int main(int argc, char *argv[])
{
    if(argc < 1){
        printf("usage: %s port\r\n", basename(argv[0]));
        return 0;
    }
    int port = atoi(argv[1]);

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons(port);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int val = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&val, sizeof(val));
    assert(listenfd >= 0);
    int ret = bind(listenfd, (const struct sockaddr*)&srv_addr, sizeof(srv_addr));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert( ret != -1);

    struct sockaddr_in client;
    socklen_t clientlen = sizeof(client);
    int fd = accept(listenfd, (struct sockaddr*)&client, &clientlen);
    if(fd < 0){
        perror("error");
        return -1;
    }else{
        char buffer[BUFFER_SIZE ];
        memset(buffer, 0, BUFFER_SIZE);
        int data_read = 0;
        int read_index = 0;
        int check_index = 0;
        int start_line = 0;
        CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;

        while(1){
            data_read = recv(fd, buffer + read_index, BUFFER_SIZE - read_index, 0);
            if(data_read == -1){
                printf("read failed\r\n");
                break;
            }else if(data_read == 0){
                printf("remote host close connection\r\n");
                break;
            }
            read_index += data_read;

            HTTP_CODE result = parse_content(buffer, check_index, checkstate, read_index, start_line);
            if(result == NO_REAUEST){
                continue;
            }else if(result == GET_REQUEST){
                printf("get content\r\n");
                send(fd,szret[0], strlen(szret[0]),0);
            }else{
                send(fd, szret[1], strlen(szret[1]), 0);
                break;
            }

        }
        close(fd);
    }
    close(listenfd);
    return 0;
}

