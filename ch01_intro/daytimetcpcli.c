#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd, n;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;
	struct sockaddr_in	cliaddr;
    socklen_t len;

	if (argc != 3)
		err_quit("usage: a.out <IPaddress> <Port>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons((uint16_t)atoi(argv[2]));	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");

    len = sizeof(cliaddr);
    if (getsockname(sockfd, (SA *) &cliaddr, &len) < 0)
        err_sys("getsockname error");
    printf("local address is %s\n", 
            sock_ntop((SA *) &cliaddr, len)); /* len have been changed by getsockname */

	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");

	exit(0);
}
