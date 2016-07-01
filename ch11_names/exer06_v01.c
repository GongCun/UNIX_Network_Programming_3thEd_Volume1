#include "mynames.h"

int 
main(int argc, char **argv)
{
	int		sockfd    , n, port, ret;
	char		recvline  [MAXLINE + 1];
	struct hostent *hptr;
	struct servent *sptr;
	struct in_addr *inetaddrp[2];
	struct in_addr **pptr;
	struct in_addr	inetaddr;
	struct sockaddr_in servaddr;
	struct sockaddr_in dest;
	socklen_t	destlen;

	if (argc != 3)
		err_quit("Usage: %s <hostname> <service>", *argv);

	ret = inet_pton(AF_INET, argv[1], &inetaddr);
	if (ret == 1) {
		inetaddrp[0] = &inetaddr;
		inetaddrp[1] = NULL;
		pptr = &inetaddrp[0];
	} else if ((hptr = gethostbyname(argv[1])) != NULL) {
		pptr = (struct in_addr **)hptr->h_addr_list;
	} else {
		err_quit("gethostbyname error: %s", hstrerror(h_errno));
	}

	if ((n = atoi(argv[2])) > 0)
		port = htons(n);
	else if ((sptr = getservbyname(argv[2], "tcp")) != NULL) {
		port = sptr->s_port;
	} else {
		err_quit("getservbyname error: %s", argv[2]);
	}

	for (; *pptr != NULL; pptr++)
		if ((sockfd = socket(hptr->h_addrtype, SOCK_DGRAM, 0)) >= 0)
			break;

	if (*pptr == NULL)
		err_sys("socket error: %s, %s", argv[1], argv[2]);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = hptr->h_addrtype;
	servaddr.sin_port = port;
	memmove(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
	Sendto(sockfd, "", 1, 0, (SA *)&servaddr, sizeof(servaddr));

	if (*pptr == NULL)
		err_quit("unable to connect");

	destlen = sizeof(struct sockaddr);
	bzero(&dest, destlen);
	n = recvfrom(sockfd, recvline, MAXLINE, 0, (SA *) & dest, &destlen);
	if (n < 0)
		err_sys("recvfrom error");
	recvline[n] = 0;
	if (fputs(recvline, stdout) == EOF)
		err_sys("fputs error");

    hptr = gethostbyaddr(&dest.sin_addr, sizeof(dest.sin_addr), dest.sin_family);
    if (hptr == NULL)
        err_quit("gethostbyaddr error: %s", hstrerror(h_errno));
    hptr = gethostbyname(hptr->h_name);
    if (hptr == NULL)
        err_quit("gethostbyname error: %s", hstrerror(h_errno));
    for (pptr = hptr->h_addr_list; *pptr != NULL; pptr++)
        printf("destination: %s\n", Inet_ntop(hptr->h_addrtype, *pptr, recvline, MAXLINE));

	exit(0);

}
