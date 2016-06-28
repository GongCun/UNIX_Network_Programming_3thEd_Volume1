#include "unproute_ex.h"
#include <netinet/udp.h>
#include <netinet/ip_var.h>
#include <netinet/udp_var.h> /* for UDPCTL_xxx constants */

int main(void)
{
    int mib[4], val;
    size_t len;
    mib[0] = CTL_NET;
    mib[1] = AF_INET;
    mib[2] = IPPROTO_UDP;
    mib[3] = UDPCTL_CHECKSUM;
    len = sizeof(val);
    if (sysctl(mib, 4, &val, &len, NULL, 0) < 0)
        err_sys("sysctl error");
    printf("udp checksum flag: %d\n", val);
    exit(0);
}
