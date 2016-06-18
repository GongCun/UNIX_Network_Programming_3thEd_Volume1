#include "unpthread_ex.h"
#include <libgen.h>

static void *doit(void *);
extern void thread_str_echo(int);

int main(int argc, char *argv[])
{
    int listenfd, *piFd;
    pthread_t tid;
    struct sockaddr_in cliaddr, servaddr;
    socklen_t clilen;

    if (argc != 2)
        err_quit("Usage: %s <port>", basename(argv[0]));

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    Bind(listenfd, (SA *)&servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);

    for (;;) {
        clilen = sizeof(cliaddr);
        piFd = malloc(sizeof(int));
        *piFd = Accept(listenfd, (SA *)&cliaddr, &clilen);
        Pthread_create(&tid, NULL, doit, (void *)piFd);
    }
    exit(0);
}

static void *doit(void *arg)
{
    int fd = *((int *)arg);
    free(arg);
    Pthread_detach(pthread_self());
    thread_str_echo(fd);
#ifndef _DEBUG
    Close(fd);
#endif
    return NULL;
}

