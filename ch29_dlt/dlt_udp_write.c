#include "udpcksum-ex.h"
#include <assert.h>

static int byte_order(void)
{
    uint32_t i;
    unsigned char *p;

    i = 0x04030201;
    p = (unsigned char *)&i;
    if (*p == 4)
        return 1; /* BIG */
    else if (*p == 1)
        return 0; /* LIT */
    else
        return -1;
} /* no use */

void dlt_udp_write(char *buf, int userlen)
{
    struct udpiphdr *pui;
    struct ip *pip;

    pip = (struct ip *)buf;
    pui = (struct udpiphdr *)buf;
    bzero(pui, sizeof(struct udpiphdr)); /* include cksum */

    /* add 8 bytes */
    pui->ui_len = htons((uint16_t)(userlen + sizeof(struct udphdr))); /* expect 32 + 8 = 40 */
#if (_DEBUG)
    printf("udp total len = %d\n", ntohs(pui->ui_len));
#endif
    /* add 28 bytes */
    userlen += sizeof(struct udpiphdr); /* expect 32 + 28 = 60 */

    pui->ui_pr = IPPROTO_UDP;
    pui->ui_src.s_addr = ((struct sockaddr_in *)local)->sin_addr.s_addr;
    pui->ui_dst.s_addr = ((struct sockaddr_in *)dest)->sin_addr.s_addr;
    pui->ui_sport = ((struct sockaddr_in *)local)->sin_port;
    pui->ui_dport = ((struct sockaddr_in *)dest)->sin_port;
    pui->ui_ulen = pui->ui_len; /* pseudo UDP header */
    if (cksum) {
        if ((pui->ui_sum = dlt_cksum((uint16_t *)pui, userlen)) == 0)
            pui->ui_sum = 0xffff;
    } else
        pui->ui_sum = 0; /* don't check sum */
#if (_DEBUG)
    printf("cksum = 0x%x\n", ntohs(pui->ui_sum));
#endif

    /* fill in rest of IP header */
    pip->ip_v = IPVERSION;
    pip->ip_hl = sizeof(struct ip) >> 2; /* /4 */
    pip->ip_tos = 0; /* type of service */
#if defined(linux) || defined(__OpenBSD__)
	pip->ip_len = htons(userlen);	/* network byte order */
#else
	pip->ip_len = userlen;			/* host byte order */
#endif
    pip->ip_id = 0; /* let IP set this */
    pip->ip_off = 0; /* frag offset, MF and DF flags */
    pip->ip_ttl = TTL_OUT;

#if (_DEBUG)
    printf("ip total len = %d\n", userlen);
    printf("local address: %s, len = %zd\n", inet_ntoa(((struct sockaddr_in *)local)->sin_addr), sizeof(*local));
    printf("dest address: %s, len = %zd\n", inet_ntoa(((struct sockaddr_in *)dest)->sin_addr), sizeof(*dest));
#endif
    if (sendto(rawfd, buf, userlen, 0, dest, destlen) != userlen)
        err_sys("sendto error");
}

