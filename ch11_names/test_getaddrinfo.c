#include "mynames.h"

int main(int argc, char **argv)
{
    int ret;
    char *addrstr;
    char *portstr;
    struct addrinfo hints, *res, *addr;

    if (argc != 3)
        err_quit("%s <Address/Name> <Port/Service>", *argv);

    addrstr = argv[1];
    portstr = argv[2];

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    ret = getaddrinfo(addrstr, portstr, &hints, &res);
    if (ret != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(ret));

    for (addr = res; addr != NULL; addr = addr->ai_next)
        printf("%s\n", Sock_ntop(addr->ai_addr, addr->ai_addrlen));

    freeaddrinfo(res);

    exit(0);

}



