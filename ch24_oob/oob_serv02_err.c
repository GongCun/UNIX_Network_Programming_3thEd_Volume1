#include "unp.h"

int main(int argc, char **argv)
{
    int n, listenfd, connfd;
    char buff[100];
    struct sockaddr_in serv;
    fd_set rset, xset;

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

    connfd = accept(listenfd, NULL, NULL);
    if (connfd < 0)
        err_sys("accept() error");

    FD_ZERO(&rset);
    FD_ZERO(&xset);

    for (;;) {
        FD_SET(connfd, &rset);
        FD_SET(connfd, &xset);

        if (select(connfd+1, &rset, NULL, &xset, NULL) < 0)
            err_sys("select error");

        if (FD_ISSET(connfd, &xset)) {
            if ((n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB)) < 0)
                err_sys("recv error");
            buff[n] = 0;
            printf("received %d OOB bytes: %s\n", n, buff);
        }

        if (FD_ISSET(connfd, &rset)) {
            if ( (n = read(connfd, buff, sizeof(buff)-1)) <= 0 ) {
                if (n < 0) err_sys("read() error");
                printf("received EOF\n");
                exit(0);
            }
            buff[n] = 0;
            printf("read %d bytes: %s\n", n, buff);
        }
    }
}

