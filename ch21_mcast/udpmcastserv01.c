#include "mymcast.h"

static void sig_chld(int signo)
{
    pid_t pid;

    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
        printf("child %d terminated\n", (int)pid);

}

int main(int argc, char *argv[])
{
    int sockfd, ret, n, replyfd;
    struct sockaddr_in servaddr, cliaddr;
    struct ip_mreq imr;
    socklen_t len;
    char buf[MAXLINE];
    time_t ticks;

    if (argc != 3)
        err_quit("Usage: %s <multicast-address> <service/port>", *argv);

    sockfd = udp_server_ip4(argv[1], argv[2], NULL);
    replyfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (replyfd < 0)
        err_sys("socket error");

    len = sizeof(servaddr);
    if (getsockname(sockfd, (SA *)&servaddr, &len) < 0)
        err_sys("getsockname error");

    /* join IPv4 multicast group */
    imr.imr_multiaddr = servaddr.sin_addr;
    imr.imr_interface.s_addr = 0; /* let system choose the interface */
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                &imr, sizeof(imr));
    if (ret == -1)
        err_sys("setsockopt error");

    if (signal(SIGCHLD, sig_chld) == SIG_ERR)
        err_sys("signal error");

    for (;;) {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, buf, MAXLINE, 0, (SA *)&cliaddr, &len);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            err_sys("recvfrom error");
        }
        printf("datagram from %s\n", Sock_ntop((SA *)&cliaddr, len));

        if (Fork() == 0) {
            ticks = time(NULL);
            snprintf(buf, MAXLINE, "%.24s\r\n", ctime(&ticks));
            n = sendto(replyfd, buf, strlen(buf), 0, (SA *)&cliaddr, len);
            if (n != strlen(buf))
                err_sys("sendto error");
            exit(0);
        }
    }

    exit(0);

}
