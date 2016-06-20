#include "oobex.h"
#include <libgen.h>

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 3)
        err_quit("usage: %s <IPaddress> <Port>", basename(argv[0]));

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");
    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
        err_sys("inet_pton error");

    if (connect(sockfd, (SA *)&servaddr, sizeof(struct sockaddr_in)) < 0)
        err_sys("connect error");

    oob_str_cli(stdin, sockfd);

    exit(0);
}
