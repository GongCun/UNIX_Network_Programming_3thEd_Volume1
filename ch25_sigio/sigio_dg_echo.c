#include "sigioex.h"
#include <sys/ioctl.h>
/* $Id$ */

static int sockfd;

#define QSIZE 8 /* size of input queue */
#define MAXDG 4096

typedef struct {
    void *dg_data; /* ptr to actual datagram */
    size_t dg_len; /* length of datagram */
    struct sockaddr *dg_sa; /* ptr to sockaddr() w/client's address */
    socklen_t dg_salen; /* length of sockaddr() */
} DG;

static DG dg[QSIZE]; /* queue of datagram to process */
static long cntread[QSIZE+1]; /* diagnostic counter */

static int iget; /* next one for main loo to process */
static int iput; /* next one for signal handler to read into */
static int nqueue; /* # on queue from main loop to process */
static socklen_t clilen; /* max length of sockaddr() */

static void sig_io(int);
static void sig_hup(int);

void sigio_dg_echo(int sockfd_arg, SA *pcliaddr, socklen_t clilen_arg)
{
    int i;
    const int on = 1;
    sigset_t zeromask, newmask, oldmask;

    sockfd = sockfd_arg;
    clilen = clilen_arg;

    for (i = 0; i < QSIZE; i++) { /* init queue of buffers */
        dg[i].dg_data = Malloc(MAXDG);
        dg[i].dg_sa = Malloc(clilen);
        dg[i].dg_salen = clilen;
    }
    iget = iput = nqueue = 0;

    Signal(SIGHUP, sig_hup);
    Signal(SIGIO, sig_io);
    Fcntl(sockfd, F_SETOWN, getpid());
    if (ioctl(sockfd, FIOASYNC, &on) < 0)
        err_sys("ioctl error");
    if (ioctl(sockfd, FIONBIO, &on) < 0)
        err_sys("ioctl error");

    Sigemptyset(&zeromask);
    Sigemptyset(&oldmask);
    Sigemptyset(&newmask);
    Sigaddset(&newmask, SIGIO); /* signal we want to block */

    Sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    for (;;) {
        while (nqueue == 0)
            sigsuspend(&zeromask);

        Sigprocmask(SIG_SETMASK, &oldmask, NULL);

        Sendto(sockfd, dg[iget].dg_data, dg[iget].dg_len, 0,
                dg[iget].dg_sa, dg[iget].dg_salen);

        if (++iget >= QSIZE)
            iget = 0;

        /* Block SIGIO */
        Sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        nqueue--;
    }
}

static void sig_io(int signo)
{
    ssize_t len;
    int nread;
    DG *ptr;

    for (nread = 0; ; ) {
        if (nqueue >= QSIZE)
            err_quit("receive overflow");

        ptr = &dg[iput];
        ptr -> dg_salen = clilen;
        len = recvfrom(sockfd, ptr->dg_data, MAXDG, 0,
                ptr->dg_sa, &ptr->dg_salen);

        if (len < 0) {
            if (errno == EWOULDBLOCK)
                break; /* all done; no more queued to read */
            else
                err_sys("recvfrom error");
        }
        ptr->dg_len = len;

        nread++;
        nqueue++;
        if (++iput >= QSIZE)
            iput = 0;
    }
    cntread[nread]++; /* histogram of # datagrams read per signal */
}

static void sig_hup(int signo)
{
    int i;

    for (i = 0; i <= QSIZE; i++)
        printf("cntread[%d] = %ld\n", i, cntread[i]);
}
