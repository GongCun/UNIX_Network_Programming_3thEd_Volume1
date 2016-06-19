#include	"unp.h"
#include <libgen.h>

static int		maxchildren;
static int		nchildren;
static pid_t	*pids;
long			*cptr, *meter(int);	/* for counting #clients/child */

#define PROCTCP "/proc/net/tcp"
#define WHITE " \t\n"
#define FIELDS 32 /* fields# */

static unsigned long sockq(int fd);


int
main(int argc, char **argv)
{
	int			listenfd, i;
	socklen_t	addrlen;
	void		sig_int(int);
	pid_t		child_make(int, int, int);
    fd_set masterset, rset;

    unsigned long qsize;

    if (argc != 4)
        err_quit("Usage: %s <port#> <#children> <#maxchildren>", basename(argv[0]));

    listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	nchildren = atoi(argv[2]);
    maxchildren = atoi(argv[3]);
	pids = Calloc(maxchildren, sizeof(pid_t));
	cptr = meter(maxchildren);

	my_lock_init("/tmp/lock.XXXXXX"); 	/* one lock file for all children */
	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, listenfd, addrlen);	/* parent returns */

	Signal(SIGINT, sig_int);

    FD_ZERO(&masterset);
    FD_SET(listenfd, &masterset);

	for ( ; ; ) {
        rset = masterset;
        if (select(listenfd+1, &rset, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            else
                err_sys("select error");
        }
        if (FD_ISSET(listenfd, &rset)) {
            if ((qsize = sockq(listenfd)) < 0)
                err_quit("qsize < 0");
#if (_DEBUG)
            printf("qsize = %ld\n", qsize);
#endif
            if (qsize > 0 && nchildren < maxchildren) {
                pids[nchildren] = child_make(nchildren, listenfd, addrlen);	/* parent returns */
                nchildren++;
            }
        }
#if (_DEBUG)
        sleep(1);
#endif
    }
}

void
sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

		/* terminate all children */
	for (i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);
	while (wait(NULL) > 0)		/* wait for all children */
		;
	if (errno != ECHILD)
		err_sys("wait error");

	pr_cpu_time();

	for (i = 0; i < nchildren; i++)
		printf("child %d, %ld connections\n", i, cptr[i]);

	exit(0);
}

static unsigned long sockq(int fd)
{
    int i, n;
    struct stat statbuf;
    char buf[1024], *array[FIELDS], *end, ptxq[16], prxq[16], *ptr;
    unsigned long long ino, procino;
    unsigned long txq, rxq;
    FILE *fp;

    snprintf(buf, sizeof(buf), "/proc/%ld/fd/%d", (long)getpid(), fd);

    if (stat(buf, &statbuf) < 0)
        err_sys("stat error");
    ino = (unsigned long long)statbuf.st_ino;

    if ((fp = fopen(PROCTCP, "r")) == NULL)
        err_sys("fopen error");

    while (fgets(buf, sizeof(buf), fp) != NULL) {
#if (__DEBUG)
        printf("buf = %s\n", buf);
#endif
        buf[strlen(buf)] = '\0';
        if ((array[0] = strtok(buf, WHITE)) == NULL)
            err_quit("strtok error");
        for (i = 1; i < FIELDS; i++) {
            if ((array[i] = strtok(NULL, WHITE)) == NULL)
                break;
        }
        if (i == 12) continue; /* header */
        end = (char *)NULL;
        if (!array[9] || !*array[9] ||
                (procino = strtoull(array[9], &end, 0)) == ULONG_MAX ||
                !end || *end)
            err_quit("read ino error");
        if (procino == ino) {
            if ((ptr = strchr(array[4], ':')) == NULL)
                err_quit("read queue error");
            n = ptr - array[4];
            strncpy(ptxq, array[4], n);
            ptxq[n] = '\0';
            n = strlen(array[4]) - strlen(ptxq) -1;
            strncpy(prxq, ptr+1, n);
            prxq[n] = '\0';
#if (_DEBUG)
            printf("ptxq = %s; prxq = %s\n", ptxq, prxq);
#endif
            end = (char *)NULL;
            if ((txq = strtoul(ptxq, &end, 16)) == ULONG_MAX ||
                    !end || *end)
                err_quit("read rcv size error");
            end = (char *)NULL;
            if ((rxq = strtoul(prxq, &end, 16)) == ULONG_MAX ||
                    !end || *end)
                err_quit("read snt size error");
            if (fclose(fp) == EOF)
                err_sys("fclose error");
#if (_DEBUG)
            printf("txq = %lu; rxq = %lu\n", txq, rxq);
#endif
            return(txq + rxq);

        }
    }

    /* expect not arrive here */
    if (ferror(fp))
        err_quit("read proc file error");
    if (fclose(fp) == EOF)
        err_sys("fclose error");
    return -1;
}

