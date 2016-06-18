#include "modeserv.h"
#include <libgen.h>
#include <assert.h>

/* Server: thread version, with mutex locking around accept() */

static void *mode_doit(void *arg);
int nthreads;
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

struct Thread *pThread;

static int listenfd;

int
main(int argc, char **argv)
{
	int					ret, i;
	struct sockaddr_in	servaddr;
	void				sig_int(int);
    pthread_t tid;
    struct Thread *arg;

    if (argc != 3)
        err_quit("Usage: %s <port> <#threads>", basename(argv[0]));

    nthreads = atoi(argv[2]);

    if ((pThread = calloc(nthreads, sizeof(struct Thread))) == NULL)
        err_sys("calloc error");

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

    for (i = 0; i < nthreads; i++) {
        arg = pThread + i;
        arg->id = i;
        if ((ret = pthread_create(&tid, NULL, &mode_doit, (void *)arg)) != 0) {
            errno = ret;
            perror("pthread_create error");
            exit(1);
        }
    }
    Signal(SIGINT, sig_int);
    for (;;)
        pause();
}

static void *mode_doit(void *arg)
{
    int ret, fd;
    socklen_t clilen, addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in cliaddr;
    struct Thread *ptr = (struct Thread *)arg;

    if ((ret = pthread_detach(pthread_self())) != 0) {
        errno = ret;
        perror("pthread_detach error");
        exit(1);
    }

    ptr->tid = pthread_self();
    printf("tid %ld (%d) starting\n", (long)ptr->tid, ptr->id);

    for (;;) {
        clilen = addrlen;
        if ((ret = pthread_mutex_lock(&mlock)) != 0) {
            errno = ret;
            perror("pthread_mutex_lock error");
            exit(1);
        }
        if ((fd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
            err_sys("accept error");
        if ((ret = pthread_mutex_unlock(&mlock)) != 0) {
            errno = ret;
            perror("pthread_mutex_unlock error");
            exit(1);
        }

        ptr->count++;
        mode_reply(fd);
        Close(fd);
    }
}

