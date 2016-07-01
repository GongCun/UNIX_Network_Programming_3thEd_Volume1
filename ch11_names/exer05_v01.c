#include "mynames.h"

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    struct hostent *hptr;
    struct servent *sptr;
    struct in_addr **pptr;
    struct sockaddr *sa;
    struct sockaddr_in servaddr;
    struct sockaddr_in6 servaddr6;
    socklen_t salen;

    if (argc != 3)
        err_quit("Usage: %s <hostname> <service>", *argv);

    if ((hptr = gethostbyname(argv[1])) == NULL) 
        err_quit("gethostbyname error: %s", argv[1]);

    if ((sptr = getservbyname(argv[2], "tcp")) == NULL) 
        err_quit("getservbyname error: %s", argv[2]);

    pptr = (struct in_addr **)hptr->h_addr_list;

    for (; *pptr != NULL; pptr++) {
        if ((sockfd = socket(hptr->h_addrtype, SOCK_STREAM, 0)) < 0)
            err_sys("socket error");

        if (hptr->h_addrtype == AF_INET) {
            salen = sizeof(servaddr);
            bzero(&servaddr, salen);
            servaddr.sin_family = hptr->h_addrtype;
            servaddr.sin_port = sptr->s_port;
            memmove(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
            sa = (SA *)&servaddr;
        } else if (hptr->h_addrtype == AF_INET6) {
            salen = sizeof(servaddr6);
            bzero(&servaddr6, salen);
            servaddr6.sin6_family = hptr->h_addrtype;
            servaddr6.sin6_port = sptr->s_port;
            memmove(&servaddr6.sin6_addr, *pptr, sizeof(struct in6_addr));
            sa = (SA *)&servaddr6;
        } else {
            err_quit("unknown communications type");
        }
        printf("trying %s\n", Sock_ntop(sa, salen));
        if (connect(sockfd, sa, salen) == 0)
            break;
        err_ret("connect error");
        Close(sockfd);
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




