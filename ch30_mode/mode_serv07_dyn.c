#include "modeserv.h"
#include <libgen.h>
#include <assert.h>

/* Server: thread version, with mutex locking around accept() */

typedef unsigned long long ull_t;

extern void Pthread_mutex_lock(pthread_mutex_t *mptr);
extern void Pthread_mutex_unlock(pthread_mutex_t *mptr);
extern int Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
			   void * (*func)(void *), void *arg);
extern void Pthread_cond_signal(pthread_cond_t *cptr);
extern void Pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr);

static void _sig_int(int signo);
static void *mode_doit(void *arg);
int nthreads, maxthreads, minthreads, nconn = 0;

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ncond = PTHREAD_COND_INITIALIZER; 

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

    if (argc != 4)
        err_quit("Usage: %s <port> <#minthreads> <#maxthreads>", basename(argv[0]));

    minthreads = nthreads = atoi(argv[2]);
    maxthreads = atoi(argv[3]);

    if ((pThread = calloc(maxthreads, sizeof(struct Thread))) == NULL)
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
    Signal(SIGINT, _sig_int);
    for (;;) {
        Pthread_mutex_lock(&nlock);
        while (nconn != nthreads) /* _not_ lock the nthreads,
                                     nthreads maybe decrease by threads */
            Pthread_cond_wait(&ncond, &nlock);
        Pthread_mutex_unlock(&nlock);

        /* all thread is busy */
        /* printf("main nconn = %d\n", nconn); */
        Pthread_mutex_lock(&mlock);

        if (nthreads >= maxthreads) {
            Pthread_mutex_unlock(&mlock);
            continue;
        }

        /* still can fork thread */
        for (i = 0; i < maxthreads; i++) {
            if (pThread[i].tid == 0) {
                pThread[i].id = i;
                Pthread_create(&tid, NULL, &mode_doit, (void *)&pThread[i]);
                break;
            }
        }
        if (i < maxthreads) /* not happend race condition */
            nthreads++;
        else
            err_quit("can't allocate threads resource");
        Pthread_mutex_unlock(&mlock);
    }
}

static void *mode_doit(void *arg)
{
    int ret, fd;
    socklen_t clilen, addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in cliaddr;
    struct Thread *ptr = (struct Thread *)arg;
    struct timeval tv;
    fd_set masterset, rset;
    FD_ZERO(&masterset);
    FD_SET(listenfd, &masterset);

    if ((ret = pthread_detach(pthread_self())) != 0) {
        errno = ret;
        perror("pthread_detach error");
        exit(1);
    }

    ptr->tid = pthread_self();
    printf("tid %llu (%d) starting\n", (ull_t)ptr->tid, ptr->id);

    for (;;) {
        rset = masterset;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        clilen = addrlen;

        Pthread_mutex_lock(&mlock);
SELECT:
        if ((ret = select(listenfd+1, &rset, NULL, NULL, &tv)) < 0) {
            if (errno == EINTR)
                goto SELECT;
            else
                err_sys("select error");
        }
        if (ret == 0) { /* no connection */
            if (nthreads > minthreads) { /* cleanup and exit */
                printf("thread %llu (%d) exit: %ld\n",
                        (ull_t)ptr->tid, 
                        ptr->id,
                        ptr->count);
                ptr->tid = 0;
                ptr->count = 0;
                nthreads--;
                Pthread_mutex_unlock(&mlock);
                pthread_exit((void *)0);
            } else {
                printf("tid (%d) timeout, continue...\n", ptr->id);
                Pthread_mutex_unlock(&mlock);
                continue;
            }
        }

        if ((fd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
            err_sys("accept error");

        if ((ret = pthread_mutex_unlock(&mlock)) != 0) {
            errno = ret;
            perror("pthread_mutex_unlock error");
            exit(1);
        }

        Pthread_mutex_lock(&nlock);
        nconn++;
        /* printf("tid (%d): nconn = %d\n", ptr->id, nconn); */
        Pthread_cond_signal(&ncond); /* only have connection, need notify main thread */
        Pthread_mutex_unlock(&nlock);

        ptr->count++;
        mode_reply(fd);
        Close(fd);

        Pthread_mutex_lock(&nlock);
        nconn--;
        /* printf("tid (%d): nconn = %d\n", ptr->id, nconn); */
        Pthread_cond_signal(&ncond); /* only dis-connection, need notify main thread */
        Pthread_mutex_unlock(&nlock);
    }
}

static void _sig_int(int signo)
{
    int i;
    for (i = 0; i < maxthreads; i++) {
        if (pThread[i].tid != 0) /* tid is a pointer to thread address, so will no be zero */
            printf(">>> tid %llu: %ld\n", (ull_t)pThread[i].tid, pThread[i].count);
    }
    mode_cpu_time();
    exit(0);
}
