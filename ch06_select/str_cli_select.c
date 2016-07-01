#include	"unp.h"

void
str_cli_select(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];
    fd_set rset;
    int n, maxfd, ret, stdineof = 0;

    maxfd = sockfd > fileno(fp) ? sockfd : fileno(fp);
    FD_ZERO(&rset);
    for (;;) {
        if(!stdineof)
            FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);

        ret = select(maxfd+1, &rset, NULL, NULL, (struct timeval *)0);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            err_sys("select error");
        }
        if (FD_ISSET(sockfd, &rset)) {
            n = Read(sockfd, recvline, MAXLINE);
            if (n == 0) {
                if (stdineof)
                    return;
                err_quit("str_cli: server terminated prematurely");
            }
            Write(fileno(stdout), recvline, n);
        }
        if (FD_ISSET(fileno(fp), &rset)) {
            n = Read(fileno(fp), sendline, MAXLINE);
            if (n == 0) {
                stdineof = 1;
                if (shutdown(sockfd, SHUT_WR) < 0)
                    err_sys("shutdown() error");
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            Writen(sockfd, sendline, n);
        }
    }
}
