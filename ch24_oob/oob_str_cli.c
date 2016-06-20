#include "oobex.h"

void oob_str_cli(FILE *fp, int sockfd)
{
    char buf[MAXLINE];
    int n;

    oob_heartbeat_cli(sockfd, 1, 5);

start:
    while ( (n = read(fileno(fp), buf, MAXLINE)) > 0 ) {
        Writen(sockfd, buf, n);
again:
        if ((n = read(sockfd, buf, MAXLINE)) <= 0) {
            if (errno == EINTR)
                goto again;
            err_quit("server terminated");
        }
        Writen(fileno(stdout), buf, n);
    }
    if (n < 0 && errno == EINTR) {
        goto start;
    } else if (n < 0)
        err_sys("read fp error");
}

