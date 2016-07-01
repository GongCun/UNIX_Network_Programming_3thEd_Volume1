#include "mynonb.h"

int connect_nonb(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tv;

    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");
    if (fcntl(sockfd, F_SETFL, flags|O_NONBLOCK) < 0)
        err_sys("fcntl F_SETFL error");

    error = 0;
    if ((n = connect(sockfd, saptr, salen)) < 0)
        if (errno != EINPROGRESS)
            return(-1);

    /* Do whatever we want while the connect is taking place. */

    if (n == 0)
        goto done;

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
    tv.tv_sec = nsec;
    tv.tv_usec = 0;

    if ((n = select(sockfd+1, &rset, &wset, NULL,
                    nsec ? &tv : NULL)) == 0) { /* timed out */
        close(sockfd);
        errno = ETIMEDOUT;
        return(-1);
    }
    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
            return(-1); /* Solaris pending error */
    } else
        err_quit("select error: sockfd not set");

done:
    if (fcntl(sockfd, F_SETFL, flags) < 0) /* restore file status flags */
        err_sys("fcntl error");

    if (error) {
        close(sockfd);
        errno = error;
        return(-1);
    }
    return(0);

}
