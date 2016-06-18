#include "modeserv.h"
#include <fcntl.h>

static int mode_lock_reg(int fd, int cmd, int type,
        off_t offset, int whence, int len)
{
    struct flock lock = {
        .l_type = type,
        .l_start = offset,
        .l_whence = whence,
        .l_len = len
    };
    return fcntl(fd, cmd, &lock);
}

void mode_lock_wait(int fd)
{
    while(mode_lock_reg(fd, F_SETLKW, F_WRLCK,
            SEEK_SET, 0, 0) < 0) {
        if (errno == EAGAIN)
            continue;
        else
            err_sys("F_SETLKW error");
    }
}

void mode_lock_release(int fd)
{
    if (mode_lock_reg(fd, F_SETLK, F_UNLCK,
            SEEK_SET, 0, 0) < 0)
        err_sys("F_UNLCK error");
}

pid_t mode_lock_stat(int fd)
{
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_start = SEEK_SET,
        .l_whence = 0,
        .l_len = 0
    };
    if (fcntl(fd, F_GETLK, &lock) < 0)
        err_sys("F_GETLK error");
    if (lock.l_type == F_UNLCK)
        return (pid_t)0;
    return lock.l_pid;
}
