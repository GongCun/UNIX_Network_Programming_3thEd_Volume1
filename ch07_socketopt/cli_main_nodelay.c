#include "unp.h"
#include <netinet/tcp.h>

extern void str_cli_write2(FILE *, int);

int
main(int argc, char **argv)
{
	int		sockfd, val;
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_quit("usage: %s <IPaddress>", *argv);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    val = 1;
    Setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	Connect(sockfd, (SA *) & servaddr, sizeof servaddr);

	str_cli_write2(stdin, sockfd);	/* do it all */

	exit(0);
}
