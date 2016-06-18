#include "modeserv.h"
#include <libgen.h>
#include <assert.h>
#include <sys/mman.h>
#include <signal.h>

/* Server: prefork, passing FD */

struct child *cptr;

int nchildren, listenfd, maxfd, minchildren, maxchildren, lock_fd=-1;
static int navail;
fd_set masterset;
volatile sig_atomic_t quitflag = 0;

static void 
sig_term(int signo)
{
	printf("process %ld terminated\n", (long)getpid());
	exit(0);
}
static void sig_chld(int signo);
static void _sig_int(int signo);

#if (AIX) /* AIX not have pselect(), but Linux and Mac OSX have */
int pselect(int nfds, fd_set *rset, fd_set *wset, fd_set *xset,
        const struct timespec *pts, const sigset_t *sigmask)
{
    int n;
    struct timeval tv;
    sigset_t saveset;

    if (ts != NULL) {
        tv.tv_sec = pts->tv_sec;
        tv.tv_usec = pts->tv_nsec / 1000;
    }
    sigprocmask(SIG_SETMASK, sigmask, &saveset);
    n = select(nfds, rset, wset, xset, (ts == NULL) ? NULL : &tv);
    sigprocmask(SIG_SETMASK, &saveset, NULL);
    return n;
}
#endif


pid_t 
mode_child_make(int i, int listenfd)
{
	int sockfd[2], connfd;
	pid_t pid;
	char c;

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sockfd) < 0)
		err_sys("socketpair error");
	if ((pid = Fork()) > 0) {	/* parent */
		Close(sockfd[1]);
		cptr[i].child_pid = pid;
		cptr[i].child_pipefd = sockfd[0];
		cptr[i].child_status = 0;
		cptr[i].child_count = 0;
		return pid;	/* parent return */
	}
	/* child continue... */
	if (setsid() < 0)
		err_sys("setsid error");

	Close(sockfd[0]);
	Close(listenfd);

	printf("child process %ld (%d) starting\n", (long)getpid(), i);
	for (;;) {
		if (mode_read_fd(sockfd[1], &c, 1, &connfd) != 1) { /* timeout */
            if (errno == EINTR)
                continue;
            else
                err_sys("read_fd error");
        }
#if (_DEBUG)
		printf("process %ld read fd %d\n", (long)getpid(), connfd);
#endif
		mode_reply(connfd);	/* process the request */
		Close(connfd);
		Write(sockfd[1], "", 1);	/* tell parent we're ready again */
	}
	exit(0);
}

int
main(int argc, char **argv)
{
	int connfd, i, nsel;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	fd_set rset;
	char c;
    sigset_t sigset;
    struct timespec ts;

	if (argc != 4)
		err_quit("Usage: %s <port> <#minchildren> <#maxchildren>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) & servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	minchildren = nchildren = atoi(argv[2]);
    maxchildren = atoi(argv[3]);

	if ((cptr = calloc(maxchildren, sizeof(struct child))) == NULL)
		err_sys("calloc error");

	Signal(SIGTERM, sig_term);

	FD_ZERO(&masterset);
	FD_SET(listenfd, &masterset);
	maxfd = listenfd;
	navail = nchildren;


	for (i = 0; i < nchildren; i++) {
		mode_child_make(i, listenfd);
		FD_SET(cptr[i].child_pipefd, &masterset);
		maxfd = max(maxfd, cptr[i].child_pipefd);
	}
	Signal(SIGINT, _sig_int);
	Signal(SIGCHLD, sig_chld);
	for (;;) {
		rset = masterset;	/* keep listenfd and child pipefd */
        if (navail <= 0) {
            if (nchildren == maxchildren)
                FD_CLR(listenfd, &rset);
            else {
                for (i = 0; i < maxchildren; i++) {
                    if (cptr[i].child_pid == 0)
                        break;
                }
                if (i == maxchildren)
                    err_sys("can't allocate process resource");
                mode_child_make(i, listenfd);
                FD_SET(cptr[i].child_pipefd, &masterset);
                rset = masterset;
                maxfd = max(maxfd, cptr[i].child_pipefd);
                navail++; nchildren++;
            }
        }

PSELECT:
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGCHLD);
        ts.tv_sec = 5; ts.tv_nsec = 0;
		if ((nsel = pselect(maxfd + 1, &rset, NULL, NULL, &ts, &sigset)) < 0) {
			if (errno == EINTR)
				goto PSELECT;
			else
				err_sys("select error");
		}
        if (nsel == 0 && nchildren > minchildren && navail >0 ) { /* timeout */
            for (i = 0; i < maxchildren; i++) {
                if (cptr[i].child_status == 0) {
                    kill(cptr[i].child_pid, SIGTERM);
				    printf(">>> exited pid %ld (%d): %ld\n",
                            (long)cptr[i].child_pid, i, cptr[i].child_count);
				    FD_CLR(cptr[i].child_pipefd, &masterset);
				    Close(cptr[i].child_pipefd);
                    cptr[i].child_pid = 0;
				    cptr[i].child_count = 0;
                    cptr[i].child_status = 0;
                    cptr[i].child_pipefd = -1;
                    if (--navail == 0 || --nchildren == minchildren)
                        break;
                }
            }
            continue;
        }
		if (FD_ISSET(listenfd, &rset)) {
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) & cliaddr, &clilen);

#if (_DEBUG)
            printf("connected client port: %d\n", ntohs(cliaddr.sin_port));
#endif

			for (i = 0; i < maxchildren; i++) {
				if (cptr[i].child_status == 0 && cptr[i].child_pid != 0)
					break;
			}
			if (i == maxchildren)
				err_quit("no available children");
			cptr[i].child_status = 1;
			cptr[i].child_count++;
			navail--;
			mode_write_fd(cptr[i].child_pipefd, &c, 1, connfd);
#if (_DEBUG)
			printf("write process %ld (%d) fd %d\n", (long)cptr[i].child_pid, i, connfd);
			/*
			 * the number of fd which parent sent, is probably
			 * different from that child received, see APUE
			 * section 17.4
			 */
#endif
			Close(connfd);
			if (--nsel == 0)
				continue;	/* no children return, all
						 * done with select() result */
		}

		for (i = 0; i < maxchildren; i++) {
            if (cptr[i].child_pid == 0)
                continue;
			if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
				if (Read(cptr[i].child_pipefd, &c, 1) <= 0) /* SOCK_DGRAM can't receive the EOF,
                                                               so use SIGCHLD to known child exited. */
					err_quit("unknown error for pid %ld (%d)", cptr[i].child_pid, i);
				cptr[i].child_status = 0;
				navail++;
				if (--nsel == 0)
					break;	/* all done */
			}
		}
	}

	exit(0);
}

static void 
sig_chld(int signo)
{
	pid_t pid;

	if (quitflag == 1)
		return;
	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
        ;
	/* expect: pid == 0, no stopped or exited process */
}

static void _sig_int(int signo)
{
    int i;
    quitflag = 1;
    for (i = 0; i < maxchildren; i++) {
        if (cptr[i].child_pid) {
            kill(cptr[i].child_pid, SIGTERM);
            printf(">>> pid %ld: %ld\n", (long)cptr[i].child_pid, cptr[i].child_count);
        }
    }

    while (wait(NULL) > 0)
        ;
    if (errno != ECHILD)
        err_sys("wait error");

    mode_cpu_time();
    exit(0);
}

