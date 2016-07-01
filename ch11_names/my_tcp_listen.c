#include "mynames.h"

int my_tcp_listen(const char *host, const char *serv, socklen_t *addrlen)
{
    int listenfd, ret;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    ret = getaddrinfo(host, serv, &hints, &res);
    if (ret != 0)
        err_quit("getaddrinfo error for %s, %s: %s",
                host, serv, gai_strerror(ret));

    ressave = res;

    do {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;
        ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (ret < 0)
            err_sys("setsockopt error");
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
        Close(listenfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("my_tcp_listen error for %s, %s", host, serv);

    Listen(listenfd, LISTENQ);

    if (addrlen)
        *addrlen = res->ai_addrlen;
    freeaddrinfo(ressave);
    return(listenfd);

}

