#include "pracudp.h"

void prac_dg_cli02(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    char *reply_addr_str, *serv_addr_str;
    struct sockaddr_in reply_addr;
    struct sockaddr_in *sin;
    socklen_t len;

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        sin = (struct sockaddr_in *)pservaddr;
        serv_addr_str = inet_ntoa(sin->sin_addr);
        printf("<send to %s>\n", (serv_addr_str == NULL) ?
                "error address" : serv_addr_str);

        len = servlen;
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, (SA *)&reply_addr, &len);
        printf("<send_to %s; reply from %s>\n", serv_addr_str,
                ((reply_addr_str = inet_ntoa(reply_addr.sin_addr)) == NULL) ?
                "error address" : reply_addr_str);

        if (len != servlen || strcmp(serv_addr_str, reply_addr_str) != 0) {
            printf("reply from %s (ignore)\n", reply_addr_str);
            continue;
        }

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }

}


