#include "unp.h"

static void
sig_pipe(int signo)
{
    printf("caught SIGPIPE\n");
    return;
}

int
main(int argc, char **argv)
{
	int		sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_quit("usage: %s <IPaddress>", *argv);

    Signal(SIGPIPE, sig_pipe);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	Connect(sockfd, (SA *) & servaddr, sizeof servaddr);

    str_cli_rst(stdin, sockfd);

	Close(sockfd);

	exit(0);
}
