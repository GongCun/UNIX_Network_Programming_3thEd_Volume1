#include "unp.h"

int listenfd, connfd = -1;

static void sig_urg(int signo)
{
    int n;
    char buff[100];

    printf("SIGURG received\n");
    n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB);

    if (n < 0)
        err_sys("recv() error %d", errno);

    buff[n] = 0;
    printf("read %d OOB byte: %s\n", n, buff);

}

int main(int argc, char **argv)
{
    int n;
    char buff[100];
    struct sockaddr_in serv;
    sigset_t sigset, oldset;

    if (signal(SIGURG, sig_urg) == SIG_ERR)
        err_sys("signal error");

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGURG);
    if (sigprocmask(SIG_BLOCK, &sigset, &oldset) < 0)
        err_sys("sigprocmask SIG_BLOCK error");

    if (argc != 2)
        err_quit("oob_serv01 <port#>");

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0) err_sys("socket() error");

    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(atoi(argv[1]));
    
    if (bind(listenfd, (SA *)&serv, sizeof(serv)) < 0)
        err_sys("bind error");

    if (listen(listenfd, 4096) < 0)
        err_sys("listen() error");


    connfd = accept(listenfd, NULL, NULL);
    if (connfd < 0)
        err_sys("accept() error");

    if (fcntl(connfd, F_SETOWN, getpid()) < 0)
        err_sys("fcntl error");

    sleep(5);
    printf("Waked up\n");

    if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0)
        err_sys("sigprocmask SIG_SETMASK error");

    for (;;) {
        if ( (n = read(connfd, buff, sizeof(buff)-1)) <= 0 ) {
            if (n < 0) err_sys("read() error");
            printf("received EOF\n");
            exit(0);
        }
        buff[n] = 0;
        printf("read %d bytes: %s\n", n, buff);
    }

}
