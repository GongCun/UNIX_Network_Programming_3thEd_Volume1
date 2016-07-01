#include "pracudp.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("Usage: %s <IPaddress>", *argv);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (!inet_aton(argv[1], &servaddr.sin_addr))
        err_sys("inet_aton error");

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    prac_dg_cli03(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);
}


