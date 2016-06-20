#include "sigioex.h"
#include <libgen.h>

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 3)
        err_quit("Usage: %s <ServerIP> <Port>", basename(argv[0]));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr) == 0)
                err_sys("inet_pton error");
    servaddr.sin_port = htons(atoi(argv[2]));

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    sigio_dg_cli(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);
}
