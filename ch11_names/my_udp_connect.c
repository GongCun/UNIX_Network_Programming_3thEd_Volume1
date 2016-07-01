#include "mynames.h"

int my_udp_connect(const char *host, const char *serv)
{
    int sockfd, ret;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    ret = getaddrinfo(host, serv, &hints, &res);
    if (ret != 0)
        err_quit("getaddrinfo errot for %s, %s: %s",
                host, serv, gai_strerror(ret));

    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;
        if ((connect(sockfd, res->ai_addr, res->ai_addrlen)) == 0)
            break;
        Close(sockfd);

    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("my_udp_connect error for %s, %s", host, serv);

    freeaddrinfo(ressave);
    return(sockfd);

}
