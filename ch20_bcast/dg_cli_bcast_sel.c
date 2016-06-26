#include "mybcast.h"

static void sig_alrm(int signo)
{
    printf("timed out\n");
}

void dg_cli_bcast_sel(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n, ret;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr reply_addr;
    const int on = 1;

    sigset_t sigset_alrm, sigset_empty;
    fd_set rset;

    sigemptyset(&sigset_empty);
    sigemptyset(&sigset_alrm);
    sigaddset(&sigset_alrm, SIGALRM);

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        err_sys("signal error");

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        Sigprocmask(SIG_BLOCK, &sigset_alrm, NULL);
        alarm(2);
        for (;;) {
            FD_ZERO(&rset);
            FD_SET(sockfd, &rset);
            ret = pselect(sockfd+1, &rset, NULL, NULL, NULL, &sigset_empty);
            if (ret == -1) {
                if (errno == EINTR)
                    break;
                err_sys("pselect error");
            } else if (ret != 1) {
                err_sys("pelect error: returned %d", ret);
            }

            /* the original signal mask is restored */
            len = servlen;
            /* sleep(5); */
            n = Recvfrom(sockfd, recvline, MAXLINE, 0, &reply_addr, &len);
            recvline[n] = 0;
            printf("from %s: %s", Sock_ntop(&reply_addr, len), recvline);
        }
    }

}


