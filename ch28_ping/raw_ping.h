#ifndef __RAW_PING_H
#define __RAW_PING_H

#include "unp.h"
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define BUFSIZE 1500

/* globals */
extern int datalen;
extern int nsent;
char *host, strHost[64], sendbuf[BUFSIZE];
pid_t pid;
int sockfd;
int recvfd;
extern struct in_addr src;
extern int src_flag;
extern int raw_hdr;
extern int xmtu;
extern int verbose;

/* function prototypes */
void *sendloop(void *);
void raw_init(void);
void raw_proc(char *, ssize_t, struct msghdr *, struct timeval *);
void raw_send(void);
void raw_send_hdr(void);
void raw_readloop(void);
void raw_sig_alrm(int);
void raw_tv_sub(struct timeval *, struct timeval *);

struct raw_proto {
    void (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
    void (*fsend)(void);
    void (*finit)(void);
    struct sockaddr *sasend; /* sockaddr{} for send, from getaddrinfo() */
    struct sockaddr *sarecv; /* sockaddr{} for receiving */
    socklen_t salen; /* length of sockaddr{} */
    int icmpproto; /* IPPROTO_xxx value for ICMP */
} *prp;

#endif
