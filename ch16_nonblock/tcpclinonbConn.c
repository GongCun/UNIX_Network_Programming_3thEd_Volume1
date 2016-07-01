#include "mynonb.h"

int
main(int argc, char **argv)
{
	int		sockfd, ret;
	struct sockaddr_in servaddr;

	if (argc != 3)
		err_quit("usage: %s <IPaddress> <Port>", *argv);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	ret = connect_nonb(sockfd, (SA *) & servaddr, sizeof servaddr, 0);
    if (ret < 0)
        err_sys("connect_nonb error");

	strclinonb(stdin, sockfd);	/* do it all */

	exit(0);
}
