#ifndef _MY_MCAST_H
#define _MY_MCAST_H

#include "unp.h"

int udp_server_ip4(const char *host, const char *serv, socklen_t *addrlenp);
void dg_cli_mcast(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);

#endif
