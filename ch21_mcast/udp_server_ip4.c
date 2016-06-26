#include "mymcast.h"

int udp_server_ip4(const char *host, const char *serv, socklen_t *addrlenp)
{
    int sockfd, ret;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_IP;

    ret = getaddrinfo(host, serv, &hints, &res);
    if (ret != 0)
        err_quit("getaddrinfo error for %s, %s: %s",
                host, serv, gai_strerror(ret));

    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;

        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        Close(sockfd);

    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("my_udp_server error for %s, %s", host, serv);

    if (addrlenp)
        *addrlenp = res->ai_addrlen;

    freeaddrinfo(ressave);

    return(sockfd);

}

