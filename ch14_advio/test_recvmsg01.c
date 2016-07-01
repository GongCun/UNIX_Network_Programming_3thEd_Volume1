#include "myadvio.h"
#include <sys/socket.h>

#define CONTROLLEN CMSG_SPACE(sizeof(struct in_addr))
static struct cmsghdr *cmptr = NULL;

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr, cliaddr;
    struct msghdr msg;
    struct iovec iov[1];
    struct cmsghdr *curptr;
    struct in_addr addr;
    const int on = 1;

    if (argc != 2)
        err_quit("Usage: %s <port>", *argv);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        err_sys("socket error");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
    if (bind(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
        err_sys("bind error");
    n = setsockopt(sockfd, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
    if (n == -1)
        err_sys("setsockopt error");


    iov[0].iov_base = recvline;
    iov[0].iov_len = sizeof(recvline);

    cmptr = malloc(CONTROLLEN);
    if (cmptr == NULL)
        err_sys("malloc error");
    cmptr->cmsg_len = CONTROLLEN;
    cmptr->cmsg_level = IPPROTO_IP;
    cmptr->cmsg_type = IP_RECVDSTADDR;

    msg.msg_name = (SA *)&cliaddr;
    msg.msg_namelen = sizeof(cliaddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;

    for (;;) {
        n = recvmsg(sockfd, &msg, MSG_WAITALL);
        if (n < 0)
            err_sys("recvmsg error");
        if (n == 0)
            break;
        recvline[n] = 0;
        printf("[received from %s] %s\n",
                Sock_ntop((SA *)&cliaddr, sizeof(cliaddr)),
                recvline);
        for (curptr = CMSG_FIRSTHDR(&msg); curptr != NULL;
                curptr = CMSG_NXTHDR(&msg, curptr)) {
            memcpy(&addr, CMSG_DATA(curptr), sizeof(addr));
            printf("\tto %s\n", inet_ntoa(addr));
        }
        if (msg.msg_flags & MSG_TRUNC)
            printf("\t(datagram truncted)\n");
    }

    exit(0);
}
