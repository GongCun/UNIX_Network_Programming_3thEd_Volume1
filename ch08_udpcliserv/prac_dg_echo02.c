#include "pracudp.h"

static void sig_int(int);
static int count;


void prac_dg_echo02(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    socklen_t len;
    char mesg[MAXLINE];

    Signal(SIGINT, sig_int);

    for (;;) {
        len = clilen;
        Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        count++;
    }

}

static void sig_int(int signo)
{
    printf("\nreceived count = %d\n", count);
    exit(0);
}
