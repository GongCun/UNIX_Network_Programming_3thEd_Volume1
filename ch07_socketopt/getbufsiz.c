#include "unp.h"

static void getbufsiz(int family, int type, int option, char *str)
{
    int sockfd, bufsiz, ret;
    socklen_t len;

    sockfd = socket(family, type, 0);
    if (sockfd == -1)
        err_sys("socket error");

    len = sizeof(bufsiz);
    ret = getsockopt(sockfd, SOL_SOCKET, option, &bufsiz, &len);
    if (ret < 0)
        err_sys("getsockopt error");

    printf("%-32s: %d\n", str, bufsiz);

}

int main(void)
{

    getbufsiz(AF_INET, SOCK_STREAM, SO_RCVBUF, "tcp receive bufsize");
    getbufsiz(AF_INET, SOCK_STREAM, SO_SNDBUF, "tcp send bufsize");
    getbufsiz(AF_INET, SOCK_DGRAM, SO_RCVBUF, "udp receive bufsize");
    getbufsiz(AF_INET, SOCK_DGRAM, SO_SNDBUF, "udp send bufsize");

    exit(0);
}

