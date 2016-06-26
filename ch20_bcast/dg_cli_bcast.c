#include "mybcast.h"

static void sig_alrm(int signo)
{
    printf("timed out\n");
}

void dg_cli_bcast(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr reply_addr;
    const int on = 1;

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        err_sys("signal error");

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);
        for (;;) {
            len = servlen;
            n = recvfrom(sockfd, recvline, MAXLINE, 0, &reply_addr, &len);
            if (n < 0) {
                if (errno == EINTR) /* caught the SIGALRM and timed out */
                    break;
                err_sys("recvfrom error");
            } else {
                recvline[n] = 0;
                printf("from %s: %s", Sock_ntop(&reply_addr, len), recvline);
            }
        }
    }

}


