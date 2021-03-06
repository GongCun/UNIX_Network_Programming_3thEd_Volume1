#include "mynonb.h"

int main(int argc, char **argv)
{
    int sockfd, listenfd, connfd, maxfd;
    int nready, n, i, maxi, client[FD_SETSIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    fd_set rset, allset;
    char buf[MAXLINE];

    int flags;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT); /* well-known port defined in unp.h == 9877 */

    Bind(listenfd, (SA *) &servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);

    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for (;;) {
        rset = allset;
        nready = Select(maxfd+1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(listenfd, &rset)) {
            printf("listening socket raedable\n");
            printf("set the listenfd to nonblocking\n");
            flags = Fcntl(listenfd, F_GETFL, 0);
            Fcntl(listenfd, F_SETFL, flags|O_NONBLOCK);
            sleep(5);
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
            if (connfd < 0) {
                if (errno == EWOULDBLOCK || errno == ECONNABORTED || 
                        errno == EPROTO || errno == EINTR) {
                    err_ret("ignore the error %d", errno);
                    printf("set the listenfd to blocking\n");
                    Fcntl(listenfd, F_SETFL, flags);
                    continue;
                } else
                    err_sys("accept error");
            }
            printf("returned from accept()\n");
            printf("set the listenfd to blocking\n");
            Fcntl(listenfd, F_SETFL, flags);
            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] == -1) {
                    client[i] = connfd;
                    break;
                }

            if (i == FD_SETSIZE)
                err_quit("too many clients");

            FD_SET(connfd, &allset);
            if (connfd > maxfd)
                maxfd = connfd;
            if (i > maxi)
                maxi = i;
            if (--nready <= 0)
                continue;
        }

        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)) {
                if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else
                    Writen(sockfd, buf, n);
                if (--nready <= 0)
                    break;
            }
        }
    }
    exit(0);
}

