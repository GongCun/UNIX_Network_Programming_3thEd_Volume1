#include "unp.h"

int main(int argc, char **argv)
{
    int n, listenfd, connfd;
    static int justreadoob = 0;
    char buff[100];
    struct sockaddr_in serv;

    struct pollfd pollfd[1];

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

    pollfd[0].fd = connfd;
    pollfd[0].events = POLLRDNORM;

    for (;;) {
        if (justreadoob == 0)
            pollfd[0].events |= POLLRDBAND;

        if (poll(pollfd, 1, -1) < 0)
            err_sys("poll error");

        if (pollfd[0].revents & POLLRDBAND) {
            if (pollfd[0].revents & POLLHUP)
                goto hungup;
            if ((n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB)) < 0)
                err_sys("recv error");
            buff[n] = 0;
            printf("received %d OOB bytes: %s\n", n, buff);
            pollfd[0].events &= ~POLLRDBAND;
            justreadoob = 1;
        }

        if (pollfd[0].revents & POLLRDNORM) {
            if ( (n = read(connfd, buff, sizeof(buff)-1)) <= 0 ) {
                if (n < 0) err_sys("read() error");
hungup:
                printf("received EOF\n");
                exit(0);
            }
            buff[n] = 0;
            printf("read %d bytes: %s\n", n, buff);
            justreadoob = 0;
        }
    }
}

