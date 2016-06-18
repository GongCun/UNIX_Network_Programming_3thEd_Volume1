#include	"unp.h"

void
str_cli_rst(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, 1);
        sleep(1);
		if (writen(sockfd, sendline+1, strlen(sendline)-1) 
                != strlen(sendline)-1)
            err_ret("writen error");

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
	}
}
