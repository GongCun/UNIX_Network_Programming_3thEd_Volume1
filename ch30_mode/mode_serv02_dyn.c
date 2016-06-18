#include "modeserv.h"
#include <libgen.h>
#include <assert.h>
#include <sys/mman.h>

/* Server: prefork, no locking */

volatile sig_atomic_t quitflag = 0;
int *nchildren, minconn, maxconn, lock_fd=-1;
static void sig_term(int signo)
{
    printf("process %ld terminated\n", (long) getpid());
    exit(0);
}
static void sig_alrm(int signo)
{
    return;
}

struct child *Pch;
static pid_t child_make(int, int);
long *Pstat;
static void _sig_int(int signo);

int
main(int argc, char **argv)
{
	int					listenfd, n;
	struct sockaddr_in	servaddr;
	void				sig_chld(int);
	void				sig_int(int);
    char c;

    if (argc != 4)
        err_quit("Usage: %s <port> <#children> <#max>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

    if ((nchildren = mmap(0, sizeof(int),
                    PROT_READ|PROT_WRITE,
                    MAP_ANON|MAP_SHARED, -1, 0)) == MAP_FAILED)
        err_sys("mmap error");

    minconn = *nchildren = atoi(argv[2]);
    maxconn = atoi(argv[3]);

    Pch = Calloc(maxconn, sizeof(struct child));
    if ((Pstat = mmap(0, sizeof(long) * maxconn,
                    PROT_READ|PROT_WRITE,
                    MAP_ANON|MAP_SHARED, -1, 0)) == MAP_FAILED)
        err_sys("mmap error");

#if 0
	Signal(SIGCHLD, sig_chld);	/* must call waitpid() */
#endif
    Signal(SIGTERM, sig_term);

    int i, nsel, maxfd = 0, nconn = 0;
    fd_set masterset, rset;

    if ((lock_fd = open("/tmp/lock.fd", O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
        err_sys("open lock_fd error");
    if (unlink("/tmp/lock.fd") < 0)
        err_sys("unlink error");

	for (i = 0; i < *nchildren; i++) {
        /* parent prefork children */
        child_make(i, listenfd);
        FD_SET(Pch[i].child_pipefd, &masterset);
        maxfd = max(maxfd, Pch[i].child_pipefd);
	}

    Signal(SIGINT, _sig_int);

    for (;;) {
        rset = masterset;
        if ((nsel = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            else
                err_sys("select error");
        } else if (nsel == 0) { /* _no_ need */
            err_msg("select timeout");
            continue;
        }
        mode_lock_wait(lock_fd);
        for (i = 0; i < maxconn; i++) {
            if (Pch[i].child_pid == 0)
                continue;
            if (FD_ISSET(Pch[i].child_pipefd, &rset)) {
                if ((n = Read(Pch[i].child_pipefd, &c, 1)) > 0) {
                    if (c == '1' && Pch[i].child_status == 0) {
                        Pch[i].child_status = 1;
                        nconn++;
                    }
                    else if (c == '0' && Pch[i].child_status == 1) {
                        Pch[i].child_status = 0;
                        nconn--;
                    }
                } else if (n == 0) {
                    /* nchildren was updated by exiting child process */
                    printf("The %ld (%d) terminated: %ld\n", (long)Pch[i].child_pid, i, Pstat[i]);
                    FD_CLR(Pch[i].child_pipefd, &masterset);
                    Pch[i].child_pid = 0;
                    Pch[i].child_pipefd = -1;
                    Pch[i].child_status = 0;
                    Pstat[i] = 0;
                    if (*nchildren < minconn) {
                        child_make(i, listenfd);
                        FD_SET(Pch[i].child_pipefd, &masterset);
                        maxfd = max(maxfd, Pch[i].child_pipefd);
                        ++(*nchildren);
                    }
                }
            }
            if (--nsel == 0)
                break;
        }
        if (nconn == *nchildren && *nchildren < maxconn) {
                for (i = 0; i < maxconn; i++) {
                    if (Pch[i].child_pid == 0)
                        break;
                }
                if (i == maxconn)
                    err_quit("no available resource");
                child_make(i, listenfd);
                FD_SET(Pch[i].child_pipefd, &masterset);
                maxfd = max(maxfd, Pch[i].child_pipefd);
                ++(*nchildren);
        }
        mode_lock_release(lock_fd);
    }
    exit(0);
}

pid_t child_make(int i, int listenfd)
{
    int fd[2];
    pid_t pid;

    if (pipe(fd) < 0)
        err_sys("pipe error");

		if ((pid = Fork()) == 0) {	/* child process */
            int connfd;
            const int opt = 1;
            struct sockaddr_in cliaddr;
            socklen_t clilen;

            close(fd[0]); /* close read */
            /* setup non-block, if the parent terminated unexpeceted,
             * write to pipe will no block
             */
            if (ioctl(fd[1], FIONBIO, &opt) < 0)
                err_sys("iotcl error");

            if (setsid() < 0) /* won't allocate
                                 controlling terminal */
                err_sys("setsid error");
            printf("process %ld starting (%d)\n", (long) getpid(), i);
		    clilen = sizeof(cliaddr);

            Signal(SIGALRM, sig_alrm);
            for (;;) {
                Write(fd[1], "0", 1); /* Free */
                alarm(5);
                if ((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
		    	    if (errno == EINTR) {
                        mode_lock_wait(lock_fd);
                        if (*nchildren > minconn) {
                            --(*nchildren);
                            kill(getpid(), SIGTERM);
                        } else {
                            mode_lock_release(lock_fd);
                            continue;		/* back to for() */
                        }
                    }
		    	    else
		    	    	err_sys("accept error");
                }
                alarm(0);
                Write(fd[1], "1", 1); /* BUSY */
                Pstat[i]++;
                mode_reply(connfd);	/* process the request */
                Close(connfd);
		    }
			exit(0);
        }
        /* parent continue */
        Close(fd[1]); /* close write */
        Pch[i].child_pid = pid;
        Pch[i].child_pipefd = fd[0];
        Pch[i].child_status = 0;
        return pid;
}

static void _sig_int(int signo)
{
    void mode_cpu_time(void);
    int i;

    quitflag = 1; /* must before kill */
    for (i = 0; i< maxconn; i++) {
        if (Pch[i].child_pid == 0)
            continue;
        kill(Pch[i].child_pid, SIGTERM);
        printf(">>> pid %ld: %ld\n", (long)Pch[i].child_pid, Pstat[i]);
    }
    while (wait(NULL) > 0)
        ;
    if (errno != ECHILD)
        err_sys("wait error");
    mode_cpu_time();
    if (munmap(Pstat, sizeof(long) * maxconn) < 0)
        err_sys("munmap error");

    exit(0);
}


