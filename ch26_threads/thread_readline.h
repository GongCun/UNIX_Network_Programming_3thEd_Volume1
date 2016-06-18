#include "unpthread_ex.h"

typedef struct {
    char buf[MAXLINE];
    int cnt;
    char *pchGlobal;
} Rline;

ssize_t Thread_readline(int fd, char *ptr, ssize_t maxlen);
ssize_t thread_readlinebuf(char **ppchUser);
void thread_readlineflush(int fd);

