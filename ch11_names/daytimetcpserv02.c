#include "mynames.h"
#include <time.h>

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t len;
    char buf[MAXLINE];
    time_t ticks;
    struct sockaddr_storage cliaddr;

    if (argc == 2)
        listenfd = my_tcp_listen(NULL, argv[1], NULL);
    else if (argc == 3)
        listenfd = my_tcp_listen(argv[1], argv[2], NULL);
    else
        err_quit("%s [ <host> ] <service/port>", *argv);


    for (;;) {
        len = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &len);
        printf("connection from %s\n", Sock_ntop((SA *)&cliaddr, len));
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buf, strlen(buf));
        Close(connfd);
    }

    exit(0);

}

