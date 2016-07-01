#include "mynames.h"

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr sa;

    if (argc != 3)
        err_quit("Usage: %s <hostname/IPaddress> <service/port>", *argv);

    sockfd = my_tcp_connect(argv[1], argv[2]);
    if (sockfd < 0) /* no make sense */
        err_quit("my_tcp_connect error");

    len = sizeof(sa);
    if (getpeername(sockfd, &sa, &len) < 0)
        err_sys("getpeername error");

    printf("connect to %s\n", Sock_ntop(&sa, len));

    while ((n = Read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }

    exit(0);

}
