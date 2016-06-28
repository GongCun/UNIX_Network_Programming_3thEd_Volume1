#ifndef _UNP_IFI_EX_H
#define _UNP_IFI_EX_H

#include "unp.h"
#include <net/if.h>
#include <libgen.h>

#define IFI_NAME 16
#define IFI_HADDR 8

struct ifi_info {
    char ifi_name[IFI_NAME];
    short ifi_index;
    short ifi_mtu;
    u_char ifi_haddr[IFI_NAME]; /* hardware address */
    u_short ifi_hlen; /* bytes in hardware address: 0,6,8 */
    short ifi_flags;
    short ifi_myflags;
    struct sockaddr *ifi_addr;
    struct sockaddr *ifi_maskaddr;
    struct sockaddr *ifi_brdaddr;
    struct sockaddr *ifi_dstaddr;
    struct ifi_info *ifi_next;
};

#define IFI_ALIAS 1 /* ifi_addr is an alias */

struct ifi_info *get_ifi_info(int, int);
struct ifi_info *Get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);

#endif
