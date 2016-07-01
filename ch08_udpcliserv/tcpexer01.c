#include "pracudp.h" 

#define RECVSZ 4096

int main(void)
{
    int sockfd, connfd, n, i;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[RECVSZ];
    socklen_t len;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(sockfd, LISTENQ);

    len = sizeof(cliaddr);
    connfd = accept(sockfd, (SA *)&cliaddr, &len);
    if (connfd < 0)
        err_sys("accept error");
    for (i = 0; i < 2; i++) {
    n = Read(connfd, mesg, RECVSZ);
    printf("%d tcp recv total bytes: %d\n", i, n);
    }

    exit(0);

}
