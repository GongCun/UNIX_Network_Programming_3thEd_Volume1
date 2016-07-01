#include	"myadvio.h"

static void sig_alrm(int signo)
{
    return;
}

void
my_dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen, long sec, long usec)
{
	int	n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];
    struct itimerval delay = {
        .it_value.tv_sec = sec,
        .it_value.tv_usec = usec,
        .it_interval.tv_sec = 0,
        .it_interval.tv_usec = 0
    };
    struct itimerval odelay;

    Signal(SIGALRM, sig_alrm);
	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        if (setitimer(ITIMER_REAL, &delay, &odelay) == -1)
            err_sys("setitimer error");
		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        if (n < 0) {
            if (errno == EINTR)
                fprintf(stderr, "socket timeout\n");
            else
                err_sys("recvfrom error");
        } else {
            if (setitimer(ITIMER_REAL, &odelay, NULL) == -1)
                err_sys("setitimer error");
		    recvline[n] = 0;	/* null terminate */
		    Fputs(recvline, stdout);
        }
	}
}
