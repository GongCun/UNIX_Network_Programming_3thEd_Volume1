#include "pracudp.h"

#undef MAXLINE
#define MAXLINE 65508

void exer08_dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int n, ret;
    socklen_t len;
    char mesg[MAXLINE];

    len = sizeof(n);
    ret = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, &len);
    if (ret == -1)
        err_sys("getsockopt error");
    printf("SO_RCVBUF = %d\n", n);

    n = 70000;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &n, sizeof(n));
    if (ret == -1)
        err_sys("setsockopt error");

    ret = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &n, &len);
    if (ret == -1)
        err_sys("getsockopt error");
    printf("SO_SNDBUF = %d\n", n);

    for (;;) {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        printf("received %d bytes\n", n);
        Sendto(sockfd, mesg, n, 0, pcliaddr, len);
        printf("sent %d bytes\n", n);
    }
}
