#include "oobex.h"
#include <libgen.h>

static void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;

    if (argc != 2)
        err_quit("usage: %s <Port>", basename(argv[0]));
    
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listenfd, (SA *)&servaddr, sizeof(struct sockaddr_in)) < 0)
        err_sys("bind error");

    if (listen(listenfd, 1024) < 0)
        err_sys("listen error");

    if (signal(SIGCHLD, sig_chld) == SIG_ERR)
        err_sys("signal SIGCHLD error");

    for (;;) {
        clilen = sizeof(struct sockaddr_in);
        if ( (connfd = accept(listenfd, (SA *)&cliaddr, &clilen)) < 0 )
            err_sys("accept error");

        if ((childpid = fork()) == 0) { /* child process */
            if (close(listenfd) < 0)
                err_sys("close error");
            oob_str_echo(connfd);
            exit(0);
        }
        if (close(connfd) < 0) /* parent closes connnected socket */
            err_sys("close error");
    }
}


