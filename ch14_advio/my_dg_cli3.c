#include	"myadvio.h"

void
my_dg_cli3(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen, long sec, long usec)
{
	int	n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];
    struct timeval tv = {
        .tv_sec = sec,
        .tv_usec = usec
    };

    n = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (n < 0)
        err_sys("setsockopt error");

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        if (n < 0) {
            if (errno == EWOULDBLOCK) {
                fprintf(stderr, "socket timeout\n");
                continue;
            }
            err_sys("recvfrom error");
        }
        recvline[n] = 0;	/* null terminate */
        Fputs(recvline, stdout);
	}
}
