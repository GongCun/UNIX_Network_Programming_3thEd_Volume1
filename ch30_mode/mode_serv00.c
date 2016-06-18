#include "modeserv.h"
#include <libgen.h>
#include <assert.h>

/* TCP Iterative Server */
int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);

    if (argc != 2)
        err_quit("Usage: %s <port>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal error");

	for (;;) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("accept error");
		}

        mode_reply(connfd); /* process the request */
		Close(connfd);
	}
    exit(0);
}
