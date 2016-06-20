#include "adv_unpifi.h"

void adv_mydg_echo(int sockfd, SA *pcliaddr, socklen_t clilen,
        SA *myaddr)
{
    int n;
    char mesg[MAXLINE];
    socklen_t len;

    for (;;) {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        printf("child %d, datagram from %s",
                getpid(), Sock_ntop(pcliaddr, len));
        printf(", to %s\n", Sock_ntop(myaddr, clilen));

        Sendto(sockfd, mesg, n, 0, pcliaddr, len);
    }

}
