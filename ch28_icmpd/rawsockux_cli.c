#include "raw_icmpd.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
    int maxfd, fd, sockfd;
    struct sockaddr_in servaddr, cliaddr;
    struct timeval tv;
    fd_set rset;

    if (argc != 3)
        err_quit("Usage: %s <IPaddress> <Port>", basename(argv[0]));

    fd = raw_cli_conn();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        err_sys("socket error");

    /* bind client's port */
    bzero(&cliaddr, sizeof(struct sockaddr_in));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons((getpid() & 0xffff) | 0x8000);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
        err_sys("bind error");

    if (raw_send_fd(fd, sockfd) < 0)
        err_sys("raw_send_fd error");

    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
        err_sys("inet_pton error");

    char sendline[MAXLINE], recvline[MAXLINE];
    int n, ret;
	while (Fgets(sendline, MAXLINE, stdin) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        FD_ZERO(&rset);
        FD_SET(sockfd, &rset);
        FD_SET(fd, &rset);
        maxfd = sockfd > fd ? sockfd : fd;
AGAIN:
        if ((ret = select(maxfd+1, &rset, NULL, NULL, &tv)) < 0) {
            if (errno == EINTR)
                goto AGAIN;
            err_sys("select error");
        }
        if (ret == 0) {
            fprintf(stderr, "socket timeout\n");
            continue;
        }
        if (FD_ISSET(sockfd, &rset)) {
		    n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		    recvline[n] = 0;	/* null terminate */
		    Fputs(recvline, stdout);
        }
        if (FD_ISSET(fd, &rset)) {
            if ((n = Read(fd, recvline, MAXLINE)) == 0)
                err_quit("ICMP daemon terminated");
            recvline[n] = 0;
            fprintf(stderr, "%s\n", recvline);
        }
	}

#ifdef _SOCKTCP
    if (send(fd, "a", 1, MSG_OOB) != 1)
        err_sys("send error");
#endif

    return 0;
}
