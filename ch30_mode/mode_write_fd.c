#include "modeserv.h"

ssize_t mode_write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
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
    pcmsg = (struct cmsghdr *)control;
    pcmsg->cmsg_level = SOL_SOCKET;
    pcmsg->cmsg_type = SCM_RIGHTS;
    pcmsg->cmsg_len = sizeof(control);
    *((int *)CMSG_DATA(pcmsg)) = sendfd;

    if ((n = sendmsg(fd, &msg, 0)) < 0)
        err_sys("sendmsg error");

    return n;
}

