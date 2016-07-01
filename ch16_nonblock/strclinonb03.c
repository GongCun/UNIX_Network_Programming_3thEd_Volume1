#include "mynonb.h"
#include <sys/wait.h>

void strclinonb(FILE *fp, int sockfd)
{
    pid_t pid;
    char buf[MAXLINE];
    int status;

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) { /* child */
        while (Readline(sockfd, buf, MAXLINE) > 0)
            Fputs(buf, stdout);
        exit(0);
    }

    /* parent continue */
    while (Fgets(buf, MAXLINE, fp) != NULL)
        Writen(sockfd, buf, strlen(buf));
    Shutdown(sockfd, SHUT_WR); /* EOF on stdin, send FIN */

    if (wait(&status) < 0)
        err_sys("wait error");
    if (WIFEXITED(status)) {
        fprintf(stderr, "child normal exit %d\n", WEXITSTATUS(status));
    } else {
        fprintf(stderr, "child abnormal exit\n");
    }

    return;
}
