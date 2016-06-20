#include "sigioex.h"
/* $Id$ */

void sig_io(int signo)
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

void sig_hup(int signo)
{
    int i;

    for (i = 0; i <= QSIZE; i++)
        printf("cntread[%d] = %ld\n", i, cntread[i]);
}
