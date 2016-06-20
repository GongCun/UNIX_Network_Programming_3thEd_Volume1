#include "unp.h"

int listenfd, connfd;

static void sig_urg(int);

int main(int argc, char **argv)
{
    int n;
    char buff[100];
    struct sockaddr_in serv;

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

    if (signal(SIGURG, sig_urg) == SIG_ERR)
        err_sys("signal error");

    if (fcntl(connfd, F_SETOWN, getpid()) < 0)
        err_sys("fcntl error");

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

static void sig_urg(int signo)
{
    int n;
    char buff[100];

    printf("SIGURG received\n");
    if ( (n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB)) < 0)
        err_sys("recv error");
    buff[n] = 0;
    printf("read %d OOB bytes: %s\n", n, buff);

}

