#include "pracudp.h"

int main(int argc, char **argv)
{
    int sockfd, n, i;
    char sendline[2048];
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("Usage: %s <IPaddress>", *argv);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (!inet_aton(argv[1], &servaddr.sin_addr))
        err_sys("inet_aton error");

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    for (i = 0; i < 2; i++) {
    n = write(sockfd, sendline, sizeof(sendline));
    if (n == -1)
        err_sys("write error");
    printf("%d: tcp sent total bytes: %d\n", i, n);
    }
    exit(0);

}
