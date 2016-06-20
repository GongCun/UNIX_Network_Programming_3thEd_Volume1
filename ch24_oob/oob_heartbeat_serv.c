#include "oobex.h"

static int servfd;
static int nsec;
static int maxnprobes;
static int nprobes;
static void sig_urg(int), sig_alrm(int);

void oob_heartbeat_serv(int servfd_arg, int nsec_arg, int maxnprobes_arg)
{
    servfd = servfd_arg; /* set global for signal handlers */
    if ((nsec = nsec_arg) < 1) nsec = 1;
    if ((maxnprobes = maxnprobes_arg) < nsec) maxnprobes = nsec;

    nprobes = 0;

    if (signal(SIGURG, sig_urg) == SIG_ERR) 
        err_sys("signal SIGURG error");
    if (fcntl(servfd, F_SETOWN, getpid()) < 0)
        err_sys("fcntl F_SETOWN");

    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        err_sys("signal SIGALRM error");
    alarm(nsec);
}

static void sig_urg(int signo)
{
    char ch;

    if (recv(servfd, &ch, 1, MSG_OOB) < 0) {
        if (errno != EWOULDBLOCK) /* the oob data haven't arrived */
            err_sys("recv error");
    }
    nprobes = 0; /* reset counter */

    if (send(servfd, &ch, 1, MSG_OOB) != 1)
        err_sys("send error");

    return; /* may interrupt client mode */
}


static void sig_alrm(int signo)
{
    if (++nprobes > maxnprobes) {
        fprintf(stderr, "no probes from client\n");
        exit(0);
    }

    alarm(nsec);
    return; /* may interrupt client mode */
}
