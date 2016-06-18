#pragma once

#include "unp.h"
#include <pcap.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp.h>

#define TTL_OUT 64

extern char *device; /* defined in main() */
extern pcap_t *ppt; /* pointer to struct pcap_t */
extern int snaplen; /* snapshot length */
extern int to_ms; /* read timeout in ms */
extern struct sockaddr *dest, *local;
extern socklen_t destlen, locallen;
extern int linktype; /* ink-layer header type */
extern int rawfd;
extern int cksum;

void dlt_open_pcap(void);
void dlt_open_raw(void);
void dlt_test_udp(void); /* sends queries and reads responses */
void dlt_sigalrm(int);
void dlt_send_dnsquery(void); /* sends a UDP query to a DNS
                                 server using a raw socket */
struct udpiphdr *dlt_udp_read(void); /* reads next packet from
                                        packet capture device. */

void dlt_udp_write(char *buf, int userlen); /* builds UDP and IP 
                                               headers and writes
                                               IP datagrams to raw
                                               socket */
struct udpiphdr *dlt_udp_read(void);
const u_char *dlt_next_pcap(int *pLen);
uint16_t dlt_cksum(uint16_t *addr, int len);

struct pseudo_udphdr {
    struct in_addr ip_src, ip_dst;
    u_char pad, ip_p;
    u_short uh_ulen; 
    struct udphdr udphdr;
    u_char data[1500];
};

