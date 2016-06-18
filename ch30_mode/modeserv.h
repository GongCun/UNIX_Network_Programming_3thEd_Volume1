#pragma once

#include "unp.h"
#include <pthread.h>

#define MAXN 16384 /* max bytes to request from server */

struct child {
    pid_t child_pid;
    int child_pipefd;
    int child_status;
    long child_count;
};

struct Thread {
    int id;
    pthread_t tid;
    long count;
};


int mode_connect(char *, char *, int); /* IPv4 */

void mode_reply(int);

void mode_cpu_time(void);

void sig_int(int);

void mode_lock_wait(int fd);
void mode_lock_release(int fd);
pid_t mode_lock_stat(int fd);

ssize_t mode_read_fd(int, void *, size_t, int *);
ssize_t mode_write_fd(int fd, void *ptr, size_t nbytes, int sendfd);

ssize_t mode_readline_r(int fd, char *vptr, ssize_t maxlen);
