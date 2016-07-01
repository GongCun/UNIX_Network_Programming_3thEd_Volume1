#include	"myadvio.h"

void
my_dg_cli2(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen, long sec, long usec)
{
	int	n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        if (my_readable_timeo(sockfd, sec, usec) <= 0) {
            fprintf(stderr, "socket timeout or error\n");
            continue;
        }
		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0;	/* null terminate */
        Fputs(recvline, stdout);
	}
}
