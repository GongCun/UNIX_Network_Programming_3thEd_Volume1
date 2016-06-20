#include "advudp.h"

FILE *fpseq;

void adv_dg_cli(FILE *fp, int sockfd, const SA *pservaddr,
socklen_t servlen)
{
    ssize_t n;
    char sendline[MAXLINE],recvline[MAXLINE+1];

    if ((fpseq = fopen("./seq.txt", "w")) == NULL)
        err_sys("fopen error");

    while (Fgets(sendline, MAXLINE, fp) != NULL) {

        n = Adv_dg_send_recv(sockfd, sendline, strlen(sendline),
                recvline, MAXLINE, pservaddr, servlen);

        recvline[n] = 0; /* null terminate */
        Fputs(recvline, stdout);
    }

}
