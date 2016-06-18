#include "modeserv.h"
#include <libgen.h>
#include <assert.h>
#include <sys/mman.h>
#include <libproc.h>
#include <sys/proc_info.h>

/* Server: prefork, no locking */

volatile sig_atomic_t quitflag = 0;
int *nchildren;
static void 
sig_term(int signo)
{
	printf("process %ld terminated\n", (long)getpid());
	exit(0);
}
static void sig_alrm(int signo)
{
    return;
}
static void sig_chld(int);

typedef void funchand(int);
funchand *_signal(int signo, funchand *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}


struct child *Pch;
static pid_t child_make(int, int);
long *Pstat;
static void _sig_int(int signo);
static int maxconn, minconn, lock_fd = -1;

int
main(int argc, char **argv)
{
	int listenfd, n;
	struct sockaddr_in servaddr;
	void sig_chld(int);
	void sig_int(int);
	struct socket_fdinfo si;

	if (argc != 4)
		err_quit("Usage: %s <port> <#children> <#max>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) & servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxconn = atoi(argv[3]);

	Pch = Calloc(maxconn, sizeof(struct child));
	if ((nchildren = mmap(0, sizeof(int),
			    PROT_READ | PROT_WRITE,
			    MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
		err_sys("mmap error");
	if ((Pstat = mmap(0, sizeof(long) * maxconn,
			  PROT_READ | PROT_WRITE,
			  MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
		err_sys("mmap error");
	minconn = *nchildren = atoi(argv[2]);

    if ((lock_fd = open("/tmp/lock.fd", O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
        err_sys("open lock_fd error");
    if (unlink("/tmp/lock.fd") < 0)
        err_sys("unlink error");

#if 0
	Signal(SIGCHLD, sig_chld);	/* must call waitpid() */
#endif
	_signal(SIGTERM, sig_term);

	int i, nsel;
	fd_set masterset, rset;
    FD_ZERO(&masterset);
    FD_SET(listenfd, &masterset);

	for (i = 0; i < *nchildren; i++) {
		/* parent prefork children */
		child_make(i, listenfd);
#if (_DEBUG)
        printf("%d status: %d\n", i, Pch[i].child_status);
#endif
	}

	_signal(SIGINT, _sig_int);
	_signal(SIGCHLD, sig_chld);

	for (;;) {
		rset = masterset;
		if ((nsel = select(listenfd+1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("select error");
		} else if (nsel == 0) {	/* _no_ need */
			err_msg("select timeout");
			continue;
		}
		if ((n = proc_pidfdinfo(getpid(), listenfd, PROC_PIDFDSOCKETINFO, &si, sizeof(si))) <= 0)
			err_sys("proc_pidfdinfo error");
        if (n < sizeof(si))
			err_quit("proc_pidfdinfo failed");
        /*
         * struct socket_info {}
         * soi_qlen: half-connction
         * soi_incqlen: incoming-connection
         * soi_rcv.sbi_cc: establish but not delivered connection
         * soi_snd.sbi_cc: send but not delivered connection
         */
        mode_lock_wait(lock_fd);
        if (si.psi.soi_qlen + si.psi.soi_incqlen > 1 &&
                *nchildren < maxconn && quitflag != 1)
#if 0
        if (si.psi.soi_rcv.sbi_cc + si.psi.soi_snd.sbi_cc > 0 &&
                *nchildren < maxconn && !quitflag)
#endif /* On BSD always 0 */
        {
            for (i = 0; i < maxconn; i++) {
                if (Pch[i].child_status == 0)
                    break;
            }
            child_make(i, listenfd);
            (*nchildren)++;
        }
        mode_lock_release(lock_fd);
	}
	exit(0);
}

pid_t 
child_make(int i, int listenfd)
{
    pid_t pid;
	if ((pid = Fork()) == 0) {	/* child process */
		int connfd;
		struct sockaddr_in cliaddr;
		socklen_t clilen;

		if (setsid() < 0)	/* won't allocate controlling
					 * terminal */
			err_sys("setsid error");
		printf("process %ld (%d) starting\n", (long)getpid(), i);
		clilen = sizeof(cliaddr);
        if (_signal(SIGALRM, sig_alrm) == SIG_ERR)
            err_sys("_signal error");
		for (;;) {
            alarm(5);
			if ((connfd = accept(listenfd, (SA *) & cliaddr, &clilen)) < 0) {
				if (errno == EINTR) {
                    mode_lock_wait(lock_fd);
                    if (*nchildren > minconn) {
                        (*nchildren)--;
#if (_DEBUG)
                        printf("nchildren = %d\n", *nchildren);
                        fflush(stdout);
#endif
                        mode_lock_release(lock_fd);
                        kill(getpid(), SIGTERM);
                    } else {
                        mode_lock_release(lock_fd);
                        continue;	/* back to for() */
                    }
                } else
					err_sys("accept error");
			}
            alarm(0);
			Pstat[i]++;
			mode_reply(connfd);	/* process the request */
			Close(connfd);
		}
		exit(0);
	}
    Pch[i].child_pid = pid;
    Pch[i].child_status = 1;
    return pid;
}

static void 
_sig_int(int signo)
{
	void mode_cpu_time(void);
	int i, ret;

	quitflag = 1;		/* must before kill */
	for (i = 0; i < maxconn; i++) {
		if ((ret = kill(Pch[i].child_pid, SIGTERM)) < 0 && errno != ESRCH)
            err_sys("kill error");
        if (ret == 0)
            printf(">>> pid %ld: %ld\n", (long)Pch[i].child_pid, Pstat[i]);
	}
	while (wait(NULL) > 0);
	if (errno != ECHILD)
		err_sys("wait error");
	mode_cpu_time();
	if (munmap(Pstat, sizeof(long) * maxconn) < 0)
		err_sys("munmap error");
	if (munmap(nchildren, sizeof(int)) < 0)
		err_sys("munmap error");

	exit(0);
}


static void sig_chld(int signo)
{
    pid_t pid;
    int i;
    if (quitflag) return;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        for (i = 0; i < maxconn; i++) {
            if (Pch[i].child_pid == pid) {
                Pch[i].child_status = 0;
                break;
            }
        }
        if (i == maxconn)
            err_quit("can't find the child: %ld", (long)pid);
    }
    return;
}
