#include	"sntp.h"

#define NTP_PORT 123

int
main(int argc, char **argv)
{
	int					sockfd, ret;
	char				buf[MAXLINE];
	ssize_t				n;
	socklen_t			salen, len;
	struct ifi_info		*ifi;
	struct sockaddr		*mcastsa, *wild, *from;
	struct timeval		now;

    struct sockaddr_in servaddr, cliaddr, ownaddr;
    struct ip_mreq imr;

	if (argc != 2)
		err_quit("usage: ssntp <IPaddress>");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr));
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
        err_sys("inet_aton error");
    servaddr.sin_port = htons(NTP_PORT); /* ntp service */

    bzero(&ownaddr, sizeof(ownaddr));
    ownaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ownaddr.sin_port = htons(NTP_PORT);
    
    if (bind(sockfd, (SA *)&ownaddr, sizeof(ownaddr)) == -1)
        err_sys("bind error");

    imr.imr_multiaddr = servaddr.sin_addr;
    imr.imr_interface.s_addr = 0;
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                &imr, sizeof(imr));
    if (ret == -1)
        err_sys("setsockopt error");

	for ( ; ; ) {
		len = sizeof(cliaddr);
		n = Recvfrom(sockfd, buf, sizeof(buf), 0, (SA *)&cliaddr, &len);
		if (gettimeofday(&now, NULL) == -1)
            err_sys("gettimeofday error");
		sntp_proc(buf, n, &now);
	}
}
