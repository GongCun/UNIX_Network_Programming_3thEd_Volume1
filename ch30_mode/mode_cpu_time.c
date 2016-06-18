#include "modeserv.h"
#include <sys/resource.h>

void mode_cpu_time(void)
{
    double user, sys;
    struct rusage selfusage, childusage;

    if (getrusage(0, &selfusage) < 0)
        err_sys("getrusage error");
    if (getrusage(-1, &childusage) < 0)
        err_sys("getrusage error");
    /*
     * time_t <= 2^31 - 1 = 2147483647 (sec) = (2^31 - 1) * 1000 msec
     * double <= 2^61 - 1, since won't overflow
     */
    user = selfusage.ru_utime.tv_sec * 1000.0 + selfusage.ru_utime.tv_usec / 1000.0;
#if (_DEBUG)
    printf ("self user = %g\n", user);
#endif
    user += childusage.ru_utime.tv_sec * 1000.0 + childusage.ru_utime.tv_usec / 1000.0;

    sys = selfusage.ru_stime.tv_sec * 1000.0 + selfusage.ru_stime.tv_usec / 1000.0;
#if (_DEBUG)
    printf ("self sys = %g\n", sys);
#endif
    sys += childusage.ru_stime.tv_sec * 1000.0 + childusage.ru_stime.tv_usec / 1000.0;

    printf("\nuser time = %g ms, sys time = %g ms \n", user, sys);
}
