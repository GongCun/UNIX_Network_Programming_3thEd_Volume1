#include "mynames.h"

int main(int argc, char **argv)
{
    int sockfd, n, port, ret;
    char recvline[MAXLINE+1];
    struct hostent *hptr;
    struct servent *sptr;
    struct in_addr *inetaddrp[2];
    struct in_addr **pptr;
    struct in_addr inetaddr;
    struct sockaddr_in servaddr;

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

    for (; *pptr != NULL; pptr++) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            err_sys("socket error");
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = port;
        memmove(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
        printf("trying %s\n", Sock_ntop((SA *)&servaddr, sizeof(servaddr)));
        if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) == 0)
            break;
        err_ret("connect error");
        close(sockfd);
    }

    if (*pptr == NULL)
        err_quit("unable to connect");

    while ((n = Read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }
    exit(0);

}




