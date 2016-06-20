#include "oobex.h"

void oob_str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

    oob_heartbeat_serv(sockfd, 1, 5);

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
        Writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("oob_str_echo error");
}
