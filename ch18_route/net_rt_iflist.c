#include "unproute_ex.h"
#include "unpifi_ex.h"

char *
net_rt_iflist(int family, int flags, size_f *lenp)
{
    int mib[6];
    char *buf;

    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;
    mib[3] = family; /* only address of this family */
    mib[4] = NET_RT_IFLIST;
    mib[5] = flags; /* interface index or 0 */

    if (sysctl(mib, 6, NULL, lenp, NULL, 0) < 0)
        return(NULL);

    if ((buf = malloc(*lenp)) == NULL)
        return(NULL);

    if (sysctl(mib, 6, buf, lenp, NULL, 0) < 0) {
        free(buf);
        return(NULL);
    }

    return(buf);
}
