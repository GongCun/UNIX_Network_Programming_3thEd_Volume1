#ifndef _ADV_UNPIFI_H
#define _ADV_UNPIFI_H

#include "unp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>

const char *
adv_inet_masktop(SA *sa, socklen_t salen);

void
adv_mydg_echo(int sockfd, SA *pcliaddr, socklen_t clilen, SA *);

#endif
