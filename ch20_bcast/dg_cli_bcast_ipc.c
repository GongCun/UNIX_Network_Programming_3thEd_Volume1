#include "mybcast.h"

static int pipefd[2];

static void sig_alrm(int signo)
{
    char str[] = "timed out\n";
    Write(pipefd[1], str, strlen(str));
    return;
}

void dg_cli_bcast_ipc(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n, maxfd;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr reply_addr;
    const int on = 1;
    fd_set rset;

    if (pipe(pipefd) == -1)
        err_sys("pipe error");
    maxfd = (sockfd > pipefd[0]) ? sockfd : pipefd[0];

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        err_sys("signal error");

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(2);
        for (;;) {
            FD_ZERO(&rset);
            FD_SET(sockfd, &rset);
            FD_SET(pipefd[0], &rset);
            n = select(maxfd+1, &rset, NULL, NULL, NULL);
            if (n < 0) {
                if (errno == EINTR)
                    continue;
                err_sys("select error");
            } 
            if (FD_ISSET(sockfd, &rset)) {
                len = servlen;
                if (sleep(5) != 0)
                    fputs("sleep interrupted\n", stderr);
                n = Recvfrom(sockfd, recvline, MAXLINE, 0, &reply_addr, &len);
                recvline[n] = 0;
                printf("from %s: %s", Sock_ntop(&reply_addr, len), recvline);
            }
            if (FD_ISSET(pipefd[0], &rset)) {
                n = Read(pipefd[0], recvline, sizeof(recvline));
                recvline[n] = 0;
                fputs(recvline, stderr);
                break;
            }
        }
    }

}


