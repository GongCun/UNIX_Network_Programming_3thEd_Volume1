#include "unp.h"

int main(int argc, char **argv)
{
    int sockfd, port;
    char *host;
    struct sockaddr_in serv;

    if (argc != 3)
        err_quit("usage: oob_cli01 <host> <port#>");

    host = argv[1];
    port = atoi(argv[2]);

    bzero(&serv, sizeof(struct sockaddr));
    serv.sin_family = AF_INET;
    if (inet_pton(AF_INET, host, &serv.sin_addr.s_addr) == 0)
        err_sys("inet_pton error");
    serv.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        err_sys("socket() error");

    if (connect(sockfd, (SA *)&serv, sizeof(struct sockaddr_in)) < 0)
        err_sys("connect() error");

    if (write(sockfd, "123", 3) != 3)
        err_sys("write error");
    printf("wrote 3 bytes of normal data\n");

    if (write(sockfd, "4", 1) != 1)
        err_sys("write error");
    printf("wrote 1 bytes of normal data\n");

    if (write(sockfd, "56", 2) != 2)
        err_sys("write error");
    printf("wrote 2 bytes of normal data\n");

    exit(0);

}
