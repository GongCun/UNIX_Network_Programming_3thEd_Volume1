#include "unp.h"

int main(int argc, char **argv)
{
    int n, listenfd, connfd, mark;
    char buff[100];
    struct sockaddr_in serv;
    const int on = 1;

    if (argc != 2)
        err_quit("oob_serv01 <port#>");

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0) err_sys("socket() error");

    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(atoi(argv[1]));
    
    if (bind(listenfd, (SA *)&serv, sizeof(serv)) < 0)
        err_sys("bind error");

    if (listen(listenfd, 4096) < 0)
        err_sys("listen() error");

    if (setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on)) < 0)
        err_sys("setsockopt SO_OOBINLINE error");

    connfd = accept(listenfd, NULL, NULL);
    if (connfd < 0)
        err_sys("accept() error");

    sleep(5);

    for (;;) {
        if ((mark = sockatmark(connfd)) < 0)
            err_sys("sockatmark error");
        if (mark)
            printf("at OOB mark\n");
        if ( (n = read(connfd, buff, sizeof(buff)-1)) <= 0 ) {
            if (n < 0) err_sys("read() error");
            printf("received EOF\n");
            exit(0);
        }
        buff[n] = 0;
        printf("read %d bytes: %s\n", n, buff);
    }

}
