#include "mybcast.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 3)
        err_quit("Usage: %s <IPaddress> <Port>", *argv);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    if (!inet_aton(argv[1], &servaddr.sin_addr))
        err_sys("inet_aton error");

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli_bcast_batch(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);
}


