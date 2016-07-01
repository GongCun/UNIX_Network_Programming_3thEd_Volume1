#include "unp.h"

int
main(int argc, char **argv)
{
	int		sockfd;
	struct sockaddr_in servaddr, cliaddr;
    int val;

	if (argc != 4)
		err_quit("usage: %s <LocalAddress> <IPaddress> <Port>", *argv);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
        err_sys("setsockopt SO_REUSEADDR error");

	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[3]));
	Inet_pton(AF_INET, argv[2], &servaddr.sin_addr);

    /*
     * Bind local:1500
     */
    bzero(&cliaddr, sizeof cliaddr);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(1500);
    if (!inet_aton(argv[1], &cliaddr.sin_addr))
        err_sys("inet_aton error");

    for (;;) {
        val = bind(sockfd, (SA *) &cliaddr, sizeof(cliaddr));
        if (val == 0)
            break;
        if (errno == EADDRINUSE)
            continue;
        err_sys("bind error");
    }

    /* Block here to test the race condition */
    printf("press any key to continue\n");
    fgetc(stdin);
    if (ferror(stdin))
        err_sys("fgetc error");
    printf("continue now\n");

retry:
	val = connect(sockfd, (SA *) & servaddr, sizeof servaddr);
    if (val < 0) {
        if (errno == EADDRINUSE)
            goto retry;
        err_sys("connect error");
    }

	str_cli(stdin, sockfd);	/* do it all */

	exit(0);
}
