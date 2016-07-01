#include "myadvio.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in sa;
    int sockfd, n;
    char buf[MAXLINE+1];

    if (argc != 5) {
        err_quit("Usage: %s <IPaddress> <Port> <sec> <usec>", *argv);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        err_sys("socket error");

    bzero(&sa, sizeof(sa));
    if (inet_aton(argv[1], &sa.sin_addr) == 0)
        err_quit("inet_aton error");
    sa.sin_port = htons(atoi(argv[2]));

    n = exer02_connect_timeo(sockfd, (SA *)&sa, sizeof(sa), atol(argv[3]), atol(argv[4]));
    if (n < 0)
        err_sys("connect_timeo error");

    while ((n = recv(sockfd, buf, MAXLINE, MSG_PEEK)) > 0) {
        printf("recv  got readable byes: %d\n", n);
        if (ioctl(sockfd, FIONREAD, &n) < 0)
            err_sys("ioctl error"); 
        printf("ioctl got readable byes: %d\n", n);
        if (n != read(sockfd, buf, n))
            err_sys("read error");
        buf[n] = 0;
        if (fputs(buf, stdout) == EOF)
            err_sys("fputs error");
    }
    if (n < 0)
        err_sys("recv error");
    exit(0);

}



    
