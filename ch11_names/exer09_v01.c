#include "mynames.h"

int main(int argc, char **argv)
{
    int sockfd;
    ssize_t n;
    char buf[MAXLINE];
    time_t ticks;
    socklen_t len;
    struct sockaddr_storage cliaddr;
    const int on = 1;

    if (argc == 2)
        sockfd = my_udp_server_reuseaddr(NULL, argv[1], NULL);
    else if (argc == 3)
        sockfd = my_udp_server_reuseaddr(argv[1], argv[2], NULL);
    else
        err_quit("Usage: %s [ <host> ] <service/port>", *argv);

    for (;;) {
        len = sizeof(cliaddr);
        n = Recvfrom(sockfd, buf, MAXLINE, 0, (SA *)&cliaddr, &len);
        printf("datagram from %s\n", Sock_ntop((SA *)&cliaddr, len));

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        Sendto(sockfd, buf, strlen(buf), 0, (SA *)&cliaddr, len);
    }

    exit(0);

}


