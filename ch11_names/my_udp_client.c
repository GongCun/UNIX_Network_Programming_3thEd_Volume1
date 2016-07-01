#include "mynames.h"

int my_udp_client(const char *host, const char *serv,
        SA **saptr, socklen_t *lenp)
{
    int sockfd, ret;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    ret = getaddrinfo(host, serv, &hints, &res);
    if (ret != 0)
        err_quit("getaddrinfo error for %s, %s: %s",
                host, serv, gai_strerror(ret));

    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd >= 0)
            break;
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("my_udp_client error for %s,%s", host, serv);

    *saptr = malloc(res->ai_addrlen);
    if (*saptr == NULL)
        err_sys("malloc error");
    *lenp = res->ai_addrlen;
    memcpy(*saptr, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(ressave);

    return(sockfd);

}
