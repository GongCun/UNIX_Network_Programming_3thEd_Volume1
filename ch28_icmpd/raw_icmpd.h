#pragma once

#include "unp.h"
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

#define SOCKNAME "/tmp/icmpd"

#define offsetof(type, f) ((size_t) \
        ((char *)&((type *)0)->f - (char *)(type *)0))

/* return listen fd */
int raw_serv_listen(void);

/* return accept fd */
int raw_serv_accept(int);

/* return client connect fd */
int raw_cli_conn(void);

/* other function prototype */
int raw_send_fd(int fd, int fd_to_send);
int raw_recv_fd(int fd); /* return recv fd */
const char * raw_icmpcode(int code);

struct cli_fd_port {
    int unixfd;
    int sport;
};
