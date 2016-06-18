#include "modeserv.h"

int mode_connect(char *hostname, char *servname, int socktype)
{
    struct addrinfo *res, hint;
    int ret, fd;

    bzero(&hint, sizeof(struct addrinfo));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_socktype = socktype;
    if ((ret = getaddrinfo(hostname, servname, &hint, &res)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(ret));

    do {
        if ((fd = socket(AF_INET, res->ai_socktype, res->ai_protocol)) < 0) {
            err_ret("socket error");
            continue;
        }
        if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
            err_ret("connect error (%d): %s:%d", errno,
                    inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr),
                    ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port));
            if (close(fd) < 0)
                err_sys("close error");
        } else
            break;
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_quit("socket/connect error");

    freeaddrinfo(res);
    return fd;
}
