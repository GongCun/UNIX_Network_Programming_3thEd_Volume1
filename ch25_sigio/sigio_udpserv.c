#include "sigioex.h"
#include <libgen.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr, cliaddr;
    int sockfd;

    if (argc != 2)
        err_quit("Usage: %s <Port>", basename(argv[0]));

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

    sigio_dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

    exit(0);

}
