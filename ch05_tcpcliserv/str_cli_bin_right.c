#include	"unp.h"
#include "./sum_right.h"

void
str_cli_bin_right(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE];
    struct args args;
    struct result result;

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
        if (sscanf(sendline, "%lu%lu",
                    (unsigned long *)&args.arg1, (unsigned long *)&args.arg2) != 2) {
            printf("invalid input: %s", sendline);
            continue;
        }

		Writen(sockfd, &args, sizeof(args));

		if (Readn(sockfd, &result, sizeof(result)) == 0)
			err_quit("str_cli: server terminated prematurely");

        printf("%lu\n", (unsigned long)result.sum);
	}
}
