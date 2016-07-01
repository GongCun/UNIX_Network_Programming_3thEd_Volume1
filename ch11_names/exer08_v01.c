#include "mynames.h"
#include <time.h>

int main(int argc, char **argv)
{
    int listenfd, connfd, ret;
    socklen_t len;
    char buf[MAXLINE];
    time_t ticks;
    struct sockaddr_storage cliaddr;
    char host[MAXLINE], serv[32];

    if (argc == 2)
        listenfd = my_tcp_listen(NULL, argv[1], NULL);
    else if (argc == 3)
        listenfd = my_tcp_listen(argv[1], argv[2], NULL);
    else
        err_quit("%s [ <host> ] <service/port>", *argv);


    for (;;) {
        len = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &len);
        ret = getnameinfo((SA*)&cliaddr, len, host, MAXLINE,
                serv, sizeof(serv), NI_NUMERICHOST|NI_NUMERICSERV);
        if (ret != 0)
            err_quit("getnameinfo error: %s", gai_strerror(ret));
        printf("connection from [%s]:%s\n", host, serv);
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buf, strlen(buf));
        Close(connfd);
    }

    exit(0);

}

