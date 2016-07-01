#include "pracudp.h"

void prac_dg_cli05(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    struct sockaddr_in cliaddr;
    socklen_t len;

    Connect(sockfd, pservaddr, servlen);

    len = sizeof(cliaddr);
    if (getsockname(sockfd, (SA *)&cliaddr, &len) == -1)
        err_sys("getsockname error");
    printf("cliaddr = %s\n", Sock_ntop((SA *)&cliaddr, len));

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, NULL, 0);
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }

}


