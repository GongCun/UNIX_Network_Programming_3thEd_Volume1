#include "myadvio.h"

int my_readable_timeo(int fd, long sec, long usec)
{
    fd_set rset;
    struct timeval tv = {
        .tv_sec = sec,
        .tv_usec = usec
    };

    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    return(select(fd+1, &rset, NULL, NULL, &tv));
    /* > 0 if description is readable */
}
