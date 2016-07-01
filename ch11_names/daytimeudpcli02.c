#include "mynames.h"

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];

    if (argc != 3)
        err_quit("Usage: %s <hostname/IPaddress> <service/port>", *argv);

    sockfd = my_udp_connect(argv[1], argv[2]);

    Sendto(sockfd, "", 1, 0, NULL, 0);
    n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
    recvline[n] = '\0';
    Fputs(recvline, stdout);

    exit(0);

}


