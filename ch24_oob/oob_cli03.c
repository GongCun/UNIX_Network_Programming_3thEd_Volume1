#include "unp.h"

int main(int argc, char **argv)
{
    int sockfd, port;
    char *host, buff[16384];
    struct sockaddr_in serv;
    const int size = 32768;

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

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) < 0)
        err_sys("setsockopt SO_SNDBUF error");

    if (write(sockfd, buff, 16384) != 16384)
        err_sys("write error");
    printf("wrote 16384 bytes of normal data\n");
    sleep(5);

    if (send(sockfd, "a", 1, MSG_OOB) != 1)
        err_sys("send error");
    printf("wrote 1 bytes of OOB data\n");

    if (write(sockfd, buff, 1024) != 1024)
        err_sys("write error");
    printf("wrote 1024 bytes of normal data\n");

    exit(0);

}
