#include	"unp.h"
#include "./sum_right.h"

void
str_echo_bin_right(int sockfd)
{
	ssize_t		n;
    struct args args;
    struct result result;

again:
	while ( (n = read(sockfd, &args, sizeof(args))) > 0) {
        result.sum = args.arg1 + args.arg2;
		Writen(sockfd, &result, sizeof(result));
    }

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}
