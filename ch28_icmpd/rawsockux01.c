#include "raw_icmpd.h"
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKNAME "/tmp/icmpd"

int main(int argc, char *argv[])
{
    struct sockaddr_un un;
    int fd, len, clifd;
    pid_t pid;

    bzero(&un, sizeof(struct sockaddr_un));

    if (unlink(SOCKNAME) < 0 && errno != ENOENT)
        err_sys("unlink error");
    strncpy(un.sun_path, SOCKNAME, sizeof(un.sun_path));
    un.sun_family = AF_UNIX;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
    if (bind(fd, (struct sockaddr *)&un, len) < 0)
        err_sys("bind error");
    printf("UNIX domain socket bound: %s\n", SOCKNAME);

    if (listen(fd, 1024) < 0)
        err_sys("listen error");

    len = sizeof(struct sockaddr_un);
    bzero(&un, len);

    if ((pid = fork()) == 0) { /* child */
        sleep(1);
        if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
            err_sys("socket error");
        un.sun_family = AF_UNIX;
        strncpy(un.sun_path, SOCKNAME, sizeof(un.sun_path));
        len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
        if (connect(fd, (struct sockaddr *)&un, len) < 0)
            err_sys("connect error");
        pause();
        exit(0);
    }

    if((clifd = accept(fd, (struct sockaddr *)&un, (socklen_t *)&len)) < 0)
        err_sys("accept error");

    /* test the buf size of UNIX domain socket */
    printf("_PC_PIPE_BUF = %ld\n", fpathconf(clifd, _PC_PIPE_BUF));

    int i, val = fcntl(clifd, F_GETFL, 0);
    if (val < 0) err_sys("fcntl F_GETFL error");
    if (fcntl(clifd, F_SETFL, val|O_NONBLOCK) < 0)
        err_sys("fcntl F_SETFL error");
    for (i=0;;i++) {
        if (write(clifd, "a", 1) < 0) {
            printf("write error: %s, errno = %d\n", strerror(errno), errno);
            break;
        }
    }
    printf("pipe buf = %d\n", i);
    kill(pid, SIGTERM);
    return 0;
}

