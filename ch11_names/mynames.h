#ifndef _MY_NAMES_H
#define _MY_NAMES_H

#include "unp.h"

int my_tcp_connect(const char *, const char *);
int my_tcp_listen(const char *, const char *, socklen_t *);
int my_tcp_listen_ipv6(const char *, const char *, socklen_t *);
int my_udp_client(const char *, const char *, struct sockaddr **, socklen_t *);
int my_udp_connect(const char *, const char *);
int my_udp_server(const char *, const char *, socklen_t *);
int my_udp_server_reuseaddr(const char *, const char *, socklen_t *);

#endif
