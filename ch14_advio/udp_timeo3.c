#include "myadvio.h"

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in sa;

    if (argc != 5) {
        err_quit("Usage: %s <IPaddress> <Port> <sec> <usec>", *argv);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        err_sys("socket error");

    bzero(&sa, sizeof(sa));
    if (inet_aton(argv[1], &sa.sin_addr) == 0)
        err_quit("inet_aton error");
    sa.sin_port = htons(atoi(argv[2]));
    my_dg_cli3(stdin, sockfd, (SA *)&sa, sizeof(sa), atol(argv[3]), atol(argv[4]));

    exit(0);

}

