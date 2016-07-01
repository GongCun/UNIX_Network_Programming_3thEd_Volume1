#include "unp.h"

int main(int argc, char **argv)
{
    int sockfd, listenfd, connfd;
    int nready, n, i, maxi;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buf[MAXLINE];
    struct pollfd *client;
    long nofiles;

    errno = 0;
    nofiles = sysconf(_SC_OPEN_MAX); /* expect 256 */
    if (nofiles < 0) {
        if (errno == 0)
            nofiles = OPEN_MAX;
        else
            err_sys("sysconf error");
    }

    client = alloca(nofiles * sizeof(struct pollfd)); /* stack-based allocation */
    if (client == NULL)
        err_sys("alloca error");

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT); /* well-known port defined in unp.h == 9877 */

    Bind(listenfd, (SA *) &servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);

    maxi = 0;
    client[0].fd = listenfd;
    client[0].events = POLLIN;
    for (i = 1; i < nofiles; i++) {
        client[i].fd = -1;
        client[i].events = POLLIN;
        client[i].revents = 0;
    }

    for (;;) {
        nready = poll(client, maxi+1, -1);
        if (nready < 0)
            err_sys("poll error");
        if (client[0].revents & POLLIN) {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
            for (i = 1; i < nofiles; i++)
                if (client[i].fd == -1) {
                    client[i].fd = connfd;
                    client[i].events = POLLIN;
                    client[i].revents = 0;
                    break;
                }
            if (i == nofiles)
                err_quit("too many clients");
            if (i > maxi)
                maxi = i;
            if (--nready <= 0)
                continue;
        }

        for (i = 1; i <= maxi; i++) {
            if ((sockfd = client[i].fd) < 0)
                continue;
            if (client[i].revents & (POLLIN|POLLERR)) {
                if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        Close(sockfd);
                        client[i].fd = -1;
                    } else
                        err_sys("read error");
                } else if (n == 0) { /* connection closed by client */
                    Close(sockfd);
                    client[i].fd = -1;
                    client[i].events = POLLIN;
                    client[i].revents = 0;
                } else
                    Writen(sockfd, buf, n);
                if (--nready <= 0)
                    break;
            }
        }
    }
    exit(0);
}

