#include "pracudp.h" 

#define RECVSZ 4096

int main(void)
{
    int sockfd, n, i;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[RECVSZ];
    socklen_t len;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

    for (i = 0; i < 2; i++) {
    len = sizeof(cliaddr);
    n = Recvfrom(sockfd, mesg, RECVSZ, 0, (SA *)&cliaddr, &len);
    printf("%d recv total bytes: %d\n", i, n);
    }

    exit(0);

}
