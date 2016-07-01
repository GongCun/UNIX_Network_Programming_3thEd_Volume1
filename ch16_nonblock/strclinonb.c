#include "mynonb.h"

static void setfl_nonb(int fd)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) == -1)
        err_sys("F_GETFL error");
    if (fcntl(fd, F_SETFL, val|O_NONBLOCK) == -1)
        err_sys("F_SETFL error");

    return;

}


void strclinonb(FILE *fp, int sockfd)
{
    int maxfd, val, stdineof, fd;
    ssize_t n, nwritten;
    fd_set rset, wset;
    char to[MAXLINE], fr[MAXLINE];
    char *tooptr, *toiptr, *froptr, *friptr;

    fd = fileno(fp);
    setfl_nonb(sockfd);
    setfl_nonb(STDOUT_FILENO);
    setfl_nonb(fd);

    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;

    maxfd = max(max(STDOUT_FILENO, sockfd), fd);
    for (;;) {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if (stdineof == 0 && toiptr < &to[MAXLINE])
            FD_SET(fd, &rset);
        if (friptr < &fr[MAXLINE])
            FD_SET(sockfd, &rset);
        if (tooptr != toiptr)
            FD_SET(sockfd, &wset);
        if (froptr != friptr)
            FD_SET(STDOUT_FILENO, &wset);
        if (select(maxfd+1, &rset, &wset, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            err_sys("select error");
        }
        /* read from fileno(fp) or sockfd */
        if (FD_ISSET(fd, &rset)) {
            if ((n = read(fd, toiptr, &to[MAXLINE]-toiptr)) < 0) {
                if (errno != EWOULDBLOCK && errno != EAGAIN)
                    err_sys("read error on fp");
            } else if (n == 0) {
                fprintf(stderr, "%s: EOF on fp\n", gf_time());
                stdineof = 1;
                if (tooptr == toiptr)
                    if (shutdown(sockfd, SHUT_WR) < 0)
                        err_sys("shutdown error");
            } else {
                fprintf(stderr, "%s: read %d bytes from stdin\n",
                        gf_time(), n);
                toiptr += n;
                FD_SET(sockfd, &wset); /* try and write to socket below */
            }
        }
        if (FD_ISSET(sockfd, &rset)) {
            if ((n = read(sockfd, friptr, &fr[MAXLINE]-friptr)) < 0) {
                if (errno != EWOULDBLOCK && errno != EAGAIN)
                    err_sys("read error on sockfd");
            } else if (n == 0) {
                fprintf(stderr, "%s: EOF on sockfd\n", gf_time());
                if (stdineof)
                    return; /* normal termination */
                else
                    err_quit("strclinonb: server terminated prematurely");
            } else {
                fprintf(stderr, "%s: read %d bytes from sockfd\n",
                        gf_time(), n);
                friptr += n;
                FD_SET(STDOUT_FILENO, &wset);
            }
        }

        /* write to stdout or sockfd */
        if (FD_ISSET(sockfd, &wset) && ( (n = toiptr - tooptr) > 0 )) {
            if ((nwritten = write(sockfd, tooptr, n)) < 0) {
                if (errno != EWOULDBLOCK && errno != EAGAIN)
                    err_sys("write error on sockfd");
            } else {
                fprintf(stderr, "%s: wrote %d bytes to sockfd\n",
                        gf_time(), nwritten);
                tooptr += nwritten;
                if (tooptr == toiptr) {
                    toiptr = tooptr = to; /* back to beginning of buffer */
                    if (stdineof)
                        if (shutdown(sockfd, SHUT_WR) < 0)
                            err_sys("shutdown error");
                }
            }
        }
        if (FD_ISSET(STDOUT_FILENO, &wset) && ( (n = friptr - froptr) > 0)) {
            if ((nwritten = write(STDOUT_FILENO, froptr, n)) < 0) {
                if (errno != EWOULDBLOCK && errno != EAGAIN)
                    err_sys("write error on stdout");
            } else {
                fprintf(stderr, "%s: wrote %d bytes to stdout\n",
                        gf_time(), nwritten);
                froptr += nwritten;
                if (froptr == friptr)
                    froptr = friptr = fr;
            }
        }
    }
}
