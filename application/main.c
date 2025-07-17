#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "msocket.h"
#include "utils.h"

#define BUF 1024
#define EXIT "exit"

void server(char* port)
{
    char buf[BUF];
    char data[BUF];
    int nread = 0;
    int sfd = 0;
    init_socket(BUF, AF_UNSPEC, 0);
    sfd = create_socket(port);
    printf("Server created\n");
    do
    {
        memset(data,0,BUF);
        nread = socket_read(sfd, buf);
        memcpy(data,buf,nread);
        printf("read: %s\n",data);
    }while(strncmp(data,EXIT,nread) != 0 && strlen(data) != 0);
}

void client(char* port)
{
    char buf[BUF];
    int sfd = 0;
    char host[256];
    
    printf("Enter desired ip: ");
    fgets(host,256,stdin);
    printf("host: %s",host);
    host[strlen(host)-1] = '\0';
    init_socket(BUF, AF_UNSPEC, 0);
    sfd = connect_socket(host, port);
    do
    {
        fgets(buf,BUF,stdin);
        printf("write: %s\n",buf);
        socket_write(sfd,buf,strlen(buf));
    }while(strcmp(buf,EXIT) != 0 && strlen(buf)-1 != 0);
}

void menu(char* argv[])
{
    if(strcmp(argv[1],"-s")==0)
    {
        printf("Entering server mode\n");
        server(argv[2]);
    }
    if(strcmp(argv[1],"-c")==0)
    {
        printf("Entering client mode\n");
        client(argv[2]);
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char ip[256];
    ret = find_my_ip(4, ip);
    if(ret == EXIT_FAILURE)
    {
        printf("Couldnt read ip\n");
        return ret;
    }
    printf("My ip: %s\n",ip);

    if(argc < 3)
    {
        printf("Usage: app < -s/-c> <port>\n");
        return EXIT_SUCCESS;
    }
    printf("option: %s, port %s\n",argv[1],argv[2]);
    menu(argv);

    return EXIT_SUCCESS;
}
