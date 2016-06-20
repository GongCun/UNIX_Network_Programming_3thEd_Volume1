#include "adv_unpifi.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

int main(void)
{
    struct ifaddrs *ifp, *ifphead;
    struct ifreq ifr, ifrcopy;
    struct ifconf ifc;
    int sockfd, len;
    char *buf;

    if (getifaddrs(&ifp) < 0)
        err_sys("getifaddrs error");

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    for (ifphead = ifp; ifp != NULL; ifp = ifp->ifa_next) {
        if (ifp->ifa_addr->sa_family == AF_INET) {
            if (ifp->ifa_flags & IFF_UP)
                printf("UP ");
            printf("%s\n", ifp->ifa_name);
            printf("inet: %s; ", Sock_ntop(ifp->ifa_addr, sizeof(*ifp->ifa_addr)));
            printf("netmask: %s\n", adv_inet_masktop(ifp->ifa_netmask, sizeof(*ifp->ifa_netmask)));

            /* Get broadcast address */
#ifdef SIOCGIFBRDADDR
            if (ifp->ifa_flags & IFF_BROADCAST) {
                bzero(&ifr, sizeof(struct ifreq));
                strncpy(ifr.ifr_name, ifp->ifa_name, IFNAMSIZ);
                if (ioctl(sockfd, SIOCGIFBRDADDR, &ifr) < 0)
                    err_sys("ioctl error");
                printf("broadcast: %s\n", Sock_ntop(&ifr.ifr_ifru.ifru_broadaddr, sizeof(struct sockaddr)));
            }
#endif
        }
    }

    freeifaddrs(ifphead);

    return (0);
}
