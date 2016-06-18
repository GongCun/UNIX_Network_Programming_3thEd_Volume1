#include "modeserv.h"
#include <libgen.h>
#include <assert.h>
#include <sys/mman.h>

/* Server: thread version, call the reentrant readline() */

static void *mode_doit(void *arg);

int
main(int argc, char **argv)
{
	int					listenfd, connfd, ret, *arg;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_int(int);
    pthread_t tid;

    if (argc != 2)
        err_quit("Usage: %s <port>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

    Signal(SIGINT, sig_int);
    for (;;) {
        clilen = sizeof(struct sockaddr_in);
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
            err_sys("accept error");
        arg = Malloc(sizeof(int));
        *arg = connfd;
        if ((ret = pthread_create(&tid, NULL, &mode_doit, (void *)arg)) != 0) {
            errno = ret;
            perror("pthread_create error");
            exit(1);
        }
    }
}

static void *mode_doit(void *arg)
{
    int ret, fd;
    fd = *((int *)arg);
    free(arg);
    if ((ret = pthread_detach(pthread_self())) != 0) {
        errno = ret;
        perror("pthread_detach error");
        exit(1);
    }

    mode_reply(fd);
    Close(fd);
    return NULL;
}
