#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr, cliaddr;
    socklen_t len;
	char				buff[MAXLINE];
	time_t				ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9999);	/* daytime server */

	/* Bind(listenfd, (SA *) &servaddr, sizeof(servaddr)); */

	Listen(listenfd, LISTENQ);

    len = sizeof(servaddr);
    Getsockname(listenfd, (SA *) &servaddr, &len); /* get the temp port */
    printf("local address is %s\n",
            sock_ntop((SA *) &servaddr, len));

	for ( ; ; ) {
        len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &len);
        printf("connection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof buff),
                ntohs(cliaddr.sin_port));

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}
