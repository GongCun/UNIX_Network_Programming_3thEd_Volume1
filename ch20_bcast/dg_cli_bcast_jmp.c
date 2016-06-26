#include "mybcast.h"
#include <setjmp.h>

static sigjmp_buf jmpbuf;

static void sig_alrm(int signo)
{
    siglongjmp(jmpbuf, 1);
}

void dg_cli_bcast_jmp(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
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

        alarm(2);
        for (;;) {
            if (sigsetjmp(jmpbuf, 1) != 0) {
                printf("timed out\n");
                break;
            }
            len = servlen;
            /* pause(); */
            /* sleep(5); */
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


