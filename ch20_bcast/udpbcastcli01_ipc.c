#include "mybcast.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("Usage: %s <IPaddress>", *argv);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    if (!inet_aton(argv[1], &servaddr.sin_addr))
        err_sys("inet_aton error");

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli_bcast_ipc(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);
}


