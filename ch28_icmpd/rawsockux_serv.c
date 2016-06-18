#include "raw_icmpd.h"
#include <sys/socket.h>
#include <sys/un.h>

#define MAXCLIENT 1024
int cursize = 0, idx = -1;
struct cli_fd_port cli_fd_port[MAXCLIENT];
void *raw_icmpd(void *);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void sig_io(int signo, siginfo_t *siginfo, void *context)
{
    int i, port;
    char c;
    pid_t pid = siginfo->si_pid;
    port = (pid & 0xffff) | 0x8000;

#ifdef _DEBUG
    printf("si_pid = %ld\n", (long)pid);
#endif

    for (i=0; i<MAXCLIENT; i++) {
        if (cli_fd_port[i].sport == port && read(cli_fd_port[i].unixfd, &c, 1) == 0) {
            idx = i;
#ifdef _DEBUG
            fprintf(stderr, ">>> signal got the port %d, pid %ld, idx %d exited\n", port, (long)pid, idx);
#endif
            break;
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    int listenfd, clifd, sockfd, val;
    pthread_t tid;
    struct sockaddr_in cliaddr;
    socklen_t len;
    char strhost[64];
    int i = 0, n, sport;
    const int on = 1;

    bzero(cli_fd_port, sizeof(struct cli_fd_port) * MAXCLIENT);

    if ((n = pthread_create(&tid, NULL, &raw_icmpd, NULL)) != 0) {
        errno = n;
        err_sys("pthread_create error");
    }

    listenfd = raw_serv_listen();

    struct sigaction act;
    bzero(&act, sizeof(struct sigaction));
    act.sa_handler = NULL;
    act.sa_sigaction = sig_io;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGIO, &act, NULL) < 0)
        err_sys("sigaction error");

    sigset_t iomask, savemask;
    sigemptyset(&iomask);
    sigaddset(&iomask, SIGIO);

    for (;;) {
        clifd = raw_serv_accept(listenfd);

        /* make write wouldn't block */
        if ((val = fcntl(clifd, F_GETFL, 0)) < 0)
            err_sys("fcntl F_GETFL error");
        if (fcntl(clifd, F_SETFL, val | O_NONBLOCK) < 0)
            err_sys("fcntl F_SETFL error");

        /* signal driver */
        if (fcntl(clifd, F_SETOWN, getpid()) < 0)
            err_sys("fcntl F_SETOWN error");
        if (ioctl(clifd, FIOASYNC, &on) < 0)
            err_sys("ioctl error");

#ifdef _DEBUG
        printf("setup O_NONBLOCK, FIOASYNC ok\n");
#endif

        sockfd = raw_recv_fd(clifd);
        len = sizeof(struct sockaddr_in);
        if (getsockname(sockfd, (struct sockaddr *)&cliaddr, &len) < 0)
            err_sys("getsockname error");
        if (inet_ntop(AF_INET, &cliaddr.sin_addr, strhost, sizeof(strhost)) == NULL)
            err_sys("inet_ntop error");
        sport = ntohs(cliaddr.sin_port);
#ifdef _DEBUG
        printf("client is %s:%d\n", strhost, sport);
#endif
        /* pad the array of cli_fd_port */
        if (sigprocmask(SIG_BLOCK, &iomask, &savemask) < 0)
            err_sys("SIG_BLOCK error");

        if ((n = pthread_mutex_lock(&mutex)) != 0) {
            errno = n;
            err_sys("pthread_mutex_lock error");
        }
        if (idx >= 0) {
#ifdef _DEBUG
            printf(">>> the client port %d closed\n", cli_fd_port[idx].sport);
#endif
            cli_fd_port[idx].unixfd = clifd;
            cli_fd_port[idx].sport = sport;
            idx = -1;
        } else {
            for (i = 0; i < MAXCLIENT; i++) {
                if (cli_fd_port[i].sport == 0) {
                    cli_fd_port[i].unixfd = clifd;
                    cli_fd_port[i].sport = sport;
                    break;
                }
            }
            if (i >= MAXCLIENT) /* TODO: need extend */
                err_quit("too many connection");
            if (i > cursize)
                cursize = i;
        }
        if ((n = pthread_mutex_unlock(&mutex)) != 0) {
            errno = n;
            err_sys("pthread_mutex_unlock error");
        }
        if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
            err_sys("SIG_SETMASK error");
    }

    return 0;
}

void *raw_icmpd(void *arg) 
{
    int icmpfd, i;
    char recvbuf[1500], errbuf[MAXLINE];
    ssize_t n;
    int iplen, subiplen, icmplen, ret;
    struct ip *ip, *subip;
    struct icmp *icmp;
    struct udphdr *udp;

    if ((icmpfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        err_sys("socket error");
    setuid(getuid());

    for (;;) {
        if ((n = recvfrom(icmpfd, recvbuf, sizeof(recvbuf), 0, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            err_sys("recvfrom error");
        }
        ip = (struct ip *)recvbuf;
        iplen = ip->ip_hl * 4;
        icmp = (struct icmp *)(recvbuf + iplen);
        if ((icmplen = n - iplen) < 8 + sizeof(struct ip))
            continue;
        subip = (struct ip *)(recvbuf + iplen + 8);
        subiplen = subip->ip_hl * 4; 
        /* icmplen >= 8 + subiplen + uh_sport + uh_dport */
        if (icmplen < 8 + subiplen + sizeof(u_short) * 2)
            continue;
        udp = (struct udphdr *)(recvbuf + iplen + 8 + subiplen);
#ifdef _DEBUG
        printf("udp->uh_sport = %d\n", ntohs(udp->uh_sport));
        printf("udp->uh_dport = %d\n", ntohs(udp->uh_dport));
        printf("%s\n", raw_icmpcode(icmp->icmp_code));
#endif

        if ((ret = pthread_mutex_lock(&mutex)) != 0) {
            errno = ret;
            err_sys("pthread_mutex_lock error");
        }
        for (i = 0; i <= cursize; i++) {
            if (ntohs(udp->uh_sport) == cli_fd_port[i].sport) {
#ifdef _DEBUG
                printf("client port %d: %s\n", cli_fd_port[i].sport, raw_icmpcode(icmp->icmp_code));
#endif
                strncpy(errbuf, raw_icmpcode(icmp->icmp_code), sizeof(errbuf));
                if (write(cli_fd_port[i].unixfd, errbuf, strlen(errbuf)) < 0
                        && errno != EAGAIN && errno != EWOULDBLOCK)
                    err_sys("write error");
                break;
            }
        }
        if ((ret = pthread_mutex_unlock(&mutex)) != 0) {
            errno = ret;
            err_sys("pthread_mutex_lock error");
        }
    }

    return NULL;
}
