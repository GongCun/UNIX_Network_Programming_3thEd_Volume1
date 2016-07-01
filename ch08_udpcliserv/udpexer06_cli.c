#include "pracudp.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    if (argc != 3)
        err_quit("Usage: %s <Server-IP> <Local-IP>", *argv);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (!inet_aton(argv[1], &servaddr.sin_addr))
        err_sys("inet_aton error");

    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0); /* kernel chooses */
    if (!inet_aton(argv[2], &cliaddr.sin_addr))
        err_sys("inet_aton error");

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    Bind(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

    prac_dg_cli(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);

}
