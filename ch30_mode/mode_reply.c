#include "modeserv.h"
/* extern ssize_t readlinebuf(void **vptrptr); */

void mode_reply(int fd)
{
    int ntowrite, done=0;
    size_t nread;
    char line[MAXLINE], reply[MAXN];
    char *ptr;
    struct sockaddr_in cliaddr;
    socklen_t addrlen;
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0)
        err_sys("setvbuf error");
    for (;;) {
#if ! (__PTHREAD)
        if ((nread  = Readline(fd, line, MAXLINE)) <= 0)
#else
        if ((nread  = mode_readline_r(fd, line, MAXLINE)) <= 0)
#endif
        {
            if (done)
                return;
            addrlen = sizeof(struct sockaddr_in);
            if (getpeername(fd, (struct sockaddr *)&cliaddr, &addrlen) != 0)
                err_sys("getpeername error");
            printf("read %zd bytes from client port: %d, done = %d, ", nread, ntohs(cliaddr.sin_port), done);
            nread = readlinebuf((void **)&ptr);
            printf("buflen = %zd\n", nread);
            fflush(stdout);
            if (nread > 0) {
                Writen(1, ptr, nread);
                puts("\n");
            }
            return;
        }
        ntowrite = atoi(line);
        if (ntowrite <= 0 || ntowrite > MAXN)
            err_quit("client request out of range: %d bytes", ntowrite);

        Writen(fd, reply, ntowrite);
        done=1;
    }
}
