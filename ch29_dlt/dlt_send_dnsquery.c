#include "udpcksum-ex.h"

void dlt_send_dnsquery(void)
{
    size_t nbytes;
    char *buf, *ptr;

    if ((buf = malloc(1500)) == NULL)
        err_sys("malloc error");
    ptr = buf + sizeof(struct udpiphdr); /* pointer to the DNS
                                            data */
    /*
     * |<ID(16)>|<QR+Opcode+AA+TC+RD(8)>|<RA+Z+RCODE(8)>|...
     */
    *((uint16_t *)ptr) = htons(1234); /* id: 2 bytes */
    ptr += 2;
    *((uint16_t *)ptr) = htons(0x0100); /* RD=1, 
                                           recursion desired */
    ptr += 2;
    /*
     * every 16 bits
     * ...|<QDCOUNT>|<ANCOUNT>|<NSCOUNT>|<ARCOUNT>|...
     *    ^
     *    ptr
     */
    *((uint16_t *)ptr) = htons(1); /* # questions */
    ptr += 2;
    *((uint16_t *)ptr) = 0;
    ptr += 2;
    *((uint16_t *)ptr) = 0;
    ptr += 2;
    *((uint16_t *)ptr) = 0;

    /*
     * question section
     * ptr
     * v
     * |<--    query name     -->|
     * | query type | query class|
     */
    ptr += 2;

    /* www.google.com */
    memcpy(ptr, "\003www\006google\003com\000", 16);
    ptr += 16;

    *((uint16_t *)ptr) = htons(1); /* query type = A */
    ptr += 2;
    *((uint16_t *)ptr) = htons(1); /* query class = 1
                                      (IP addr) */
    ptr += 2; /* end */

    nbytes = (ptr - buf) - sizeof(struct udpiphdr);
    printf("send %zd bytes of data\n", nbytes);
    dlt_udp_write(buf, nbytes); /* buf point to 
                                   the begin of packet */
}
