#include "unp.h"
#include "modeserv.h"
#include <libgen.h>
#include <assert.h>


int main(int argc, char *argv[])
{
    if (argc != 6)
        err_quit("Usage: %s <hostname or IPaddr> <port> <#children> "
                "<#loop/child> <#bytes/request>", basename(argv[0]));
    int nchildren = atoi(argv[3]);
    int nloops = atoi(argv[4]);
    int nbytes = atoi(argv[5]);
    if (nbytes >= MAXN || nbytes <=0)
        err_quit("nbytes = %d", nbytes);
    char request[MAXLINE], reply[MAXN];
    snprintf(request, sizeof(request), "%d\n", nbytes);
    int i, j, fd;
    size_t n, nw;
    pid_t pid;

    for (i = 0; i < nchildren; i++) {
        if ((pid = fork()) < 0) err_sys("fork error");
        if (pid == 0) { /* child */
            for (j = 0; j < nloops; j++) {
                fd = mode_connect(argv[1], argv[2], SOCK_STREAM);
                if ((nw = write(fd, request, strlen(request))) != strlen(request))
                    err_sys("write error");
                if ((n = Readn(fd, reply, nbytes)) != nbytes) {
                    struct sockaddr_in localaddr;
                    socklen_t addrlen = sizeof(struct sockaddr_in);
                    if (getsockname(fd, (struct sockaddr *)&localaddr, &addrlen) < 0)
                        err_sys("getsockname error");
                    err_quit("child (%d): server returned %d bytes: client port: %d (sent %zd bytes)", 
                            i, n, ntohs(localaddr.sin_port), nw);
                }
                if (close(fd) < 0)
                    err_sys("close error");
            }
            printf("child %d done\n", i);
            exit(0);
        }
        /* parent loops to fork() again */
    }

    while (wait(NULL) > 0)
        ;
    if (errno != ECHILD)
        err_sys("wait error");
    exit(0);
}

