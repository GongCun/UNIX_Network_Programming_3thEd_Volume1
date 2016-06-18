#include "modeserv.h"
#include <libgen.h>
#include <assert.h>
#include <sys/mman.h>

/* Server: prefork, file locking using fcntl() */

int nchildren;
pid_t *ppids;
static void sig_term(int signo)
{
    printf("process %ld terminated\n", (long) getpid());
    exit(0);
}

long *pservice; /* a array record the service count of
                   each child */

static int lock_fd = -1;
static void sig_alrm(int signo)
{
    pid_t pid;
    if ((pid = mode_lock_stat(lock_fd)) == 0)
        printf("no process lock\n");
    else
        printf("process %ld lock\n", (long)pid);
    return;
}

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
	void				sig_int(int);

    if (argc != 3)
        err_quit("Usage: %s <port> <#children>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
    nchildren = atoi(argv[2]);
    /* save the child pid to allow the main() to
     * terminate all the children
     */
    if ((ppids = calloc(nchildren, sizeof(pid_t))) == NULL)
        err_sys("calloc error");

#if 0
    int fd;
    if ((fd = open("/dev/zero", O_RDWR, 0)) < 0)
        err_sys("open error");
    if ((pservice = mmap(0, sizeof(long) * nchildren,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED, fd, 0)) == MAP_FAILED)
        err_sys("mmap error");
    if (close(fd) < 0)
        err_sys("close error");
#endif /* Mac OS X doesn't support the /dev/zero mapping */
    if ((pservice = mmap(0, sizeof(long) * nchildren,
                    PROT_READ|PROT_WRITE,
                    MAP_ANON|MAP_SHARED, -1, 0)) == MAP_FAILED)
        err_sys("mmap error");

    if ((lock_fd = open("/tmp/lock.fd", O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
        err_sys("open lock_fd error");
    if (unlink("/tmp/lock.fd") < 0)
        err_sys("unlink error");

#if 0
	Signal(SIGCHLD, sig_chld);	/* must call waitpid() */
#endif
    Signal(SIGTERM, sig_term);
    Signal(SIGALRM, sig_alrm);

    int i;

	for (i = 0; i < nchildren; i++) {
		if ((ppids[i] = Fork()) == 0) {	/* child process */
            if (setsid() < 0) /* won't allocate
                                 controlling terminal */
                err_sys("setsid error");
            printf("process %ld starting\n", (long) getpid());
            for (;;) {
		        clilen = sizeof(cliaddr);
                mode_lock_wait(lock_fd);
                if ((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
		    	    if (errno == EINTR)
		    		    continue;		/* back to for() */
		    	    else
		    	    	err_sys("accept error");
                }
                mode_lock_release(lock_fd);
                pservice[i]++;
                mode_reply(connfd);	/* process the request */
                Close(connfd);
		    }
			exit(0);
		}
        /* parent prefork children */
	}

    Signal(SIGINT, sig_int);

    for (;;)
        pause(); /* everay done by children */
    exit(0);
}
