#include "modeserv.h"

ssize_t mode_read_fd(int fd, void *ptr, size_t nbytes, int *pfd)
{
    struct iovec iov[1];
    struct msghdr msg;
    struct cmsghdr *pcmsg;
    int n;
    u_char control[sizeof(struct cmsghdr) + sizeof(int)];

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    if ((n = recvmsg(fd, &msg, 0)) < 0)
        return n;

    if ((pcmsg = CMSG_FIRSTHDR(&msg)) != NULL &&
            pcmsg->cmsg_len ==  CMSG_LEN(sizeof(int))) {
        if (pcmsg->cmsg_level != SOL_SOCKET)
            err_quit("control level != SOL_SOCKET");
        if (pcmsg->cmsg_type != SCM_RIGHTS)
            err_quit("control type != SCM_RIGHTS");
        *pfd = *((int *)CMSG_DATA(pcmsg));
    } else 
        err_quit("control len != CMSG_LEN(sizeof(int))");
    return n;
}

