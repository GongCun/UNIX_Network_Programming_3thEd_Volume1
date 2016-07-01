#include "unp.h"
#include <netinet/tcp.h>

static int getrcvbuf(int);
static int getmssval(int);

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE];
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("Usage: %s <IPaddress>", argv[0]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_quit("inet_pton error for %s", argv[1]);

    /*
     * Get before connect.
     */
    printf("before connect: recvbuf = %d; MSS = %d\n",
            getrcvbuf(sockfd), getmssval(sockfd));

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");

    /* Get after connect */
    printf("after connect: recvbuf = %d; MSS = %d\n",
            getrcvbuf(sockfd), getmssval(sockfd));

    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }
    if (n < 0)
        err_sys("read error");
    exit(0);
}

static int getrcvbuf(int sockfd)
{
    int value;
    socklen_t len;

    len = sizeof(value);
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &value, &len) < 0)
        err_sys("getsockopt error");

    return value;
}

static int getmssval(int sockfd)
{
    int value;
    socklen_t len;

    len = sizeof(value);
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &value, &len) < 0)
        err_sys("getsockopt error");

    return value;
}
