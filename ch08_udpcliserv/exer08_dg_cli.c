#include "pracudp.h"

#undef MAXLINE

void exer08_dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n, ret;
    char *sendline, *recvline, *ptr;
    int MAXLINE = 65507; /* for IPv4 Maximum sendbuf size */

    sendline = Malloc(MAXLINE);
    recvline = Malloc(MAXLINE + 1);

    n = 70000;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &n, sizeof(n));
    if (ret == -1)
        err_sys("setsockopt error");
    n = 70000;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
    if (ret == -1)
        err_sys("setsockopt error");

    if ((ptr = getenv("MAXLINE")) != NULL)
        MAXLINE = atoi(ptr);

    printf("MAXLINE = %d\n", MAXLINE);

    n = sendto(sockfd, sendline, MAXLINE, 0, pservaddr, servlen);
    if (n != MAXLINE)
        err_sys("sendto error");
    printf("sent %d bytes\n", n);
    
    n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
    printf("received %d bytes\n", n);

    free(sendline);
    free(recvline);

}


