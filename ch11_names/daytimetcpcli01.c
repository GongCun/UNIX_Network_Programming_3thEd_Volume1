#include "mynames.h"

int main(int argc, char **argv)
{
    int sockfd, n, port;
    char recvline[MAXLINE+1];
    struct hostent *hptr;
    struct servent *sptr;
    struct in_addr *inetaddrp[2];
    struct in_addr **pptr;
    struct in_addr inetaddr;
    struct sockaddr_in servaddr;

    if (argc != 3)
        err_quit("Usage: %s <hostname> <service>", *argv);

retry:
    hptr = gethostbyname(argv[1]);
    if (hptr == NULL) {
        if (h_errno == HOST_NOT_FOUND) {
            if (inet_aton(argv[1], &inetaddr) == 0)
                err_sys("inet_aton error");
            inetaddrp[0] = &inetaddr;
            inetaddrp[1] = NULL;
            pptr = inetaddrp;
        } else if (h_errno == TRY_AGAIN) {
            goto retry;
        } else {
            err_quit("gethostbyname error: host %s: %s", argv[1], hstrerror(h_errno));
        }
    } else {
        pptr = (struct in_addr **)hptr->h_addr_list;
    }

    errno = 0;
    sptr = getservbyname(argv[2], "tcp");
    if (sptr == NULL) {
        if (errno == 0) {
            port = htons(atoi(argv[2])); /* network byte order */
        } else {
            err_sys("getservbyname error");
        }
    } else {
        port = sptr->s_port;
    }

    for (; *pptr != NULL; pptr++) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            err_sys("socket error");
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = port;
        memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
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




