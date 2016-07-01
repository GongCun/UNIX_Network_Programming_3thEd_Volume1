#include "unp.h"

static void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid((pid_t)-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    if (pid == (pid_t)-1)
        if (errno != ECHILD)
            err_sys("waitpid error");
    return;

}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    if (Signal_intr(SIGCHLD, sig_chld) == SIG_ERR) /* POSIX sigaction implements
                                                      SA_INTERRUPT */
        err_sys("Signal error");

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT); /* well-known port defined in unp.h == 9877 */

    Bind(listenfd, (SA *) &servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
        if (connfd < 0) {
            if (errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }
        if ((childpid = fork()) == 0) { /* child */
            Close(listenfd);
            str_echo(connfd); /* process the request */
            exit(0);
        }
        Close(connfd); /* parent closes connected socket */
    }

    exit(0);
}

