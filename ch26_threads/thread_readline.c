#include "thread_readline.h"
#include <assert.h>

#if defined _NO_THREAD

/*---------------------------------------------------------------
 * It's original version for _no_ thread-safe version of readline
 */


static char buf[MAXLINE];
static int cnt;
static char *pchGlobal;

static ssize_t thread_my_read(int fd, char *pchUser)
{
    if (cnt <= 0) {
again:
        if ((cnt = read(fd, buf, MAXLINE)) < 0) {
            if (errno == EINTR)
                goto again;
            return -1;
        } else if (cnt == 0)
            return 0;
        pchGlobal = buf;
    }
    cnt--;
    *pchUser = *pchGlobal++;
    return 1;
}

ssize_t thread_readline(int fd, char *ptr, ssize_t maxlen)
{
    ssize_t i, rc;
    char ch, *pch;

    pch = ptr;
    for (i = 1; i < maxlen; i++) {
        if ((rc = thread_my_read(fd, &ch)) == 1) {
            *pch++ = ch;
            if (ch == '\n')
                break;
        } else if (rc == 0) {
            *pch = 0;
            return i-1;
        } else
            return -1;
    }

    *pch = 0;
    return i;
}

ssize_t Thread_readline(int fd, char *ptr, ssize_t maxlen) {
    ssize_t len;
    len = thread_readline(fd, ptr, maxlen);
    if (len < 0)
        err_sys("thread_readline error");
    return len;
}

ssize_t thread_readlinebuf(char **ppchUser)
{
    if (cnt)
        *ppchUser = pchGlobal;
    return cnt;
}

void thread_readlineflush(int fd)
{
    char *pch;
    ssize_t restcnt;

    restcnt = thread_readlinebuf(&pch);
    if (restcnt) {
        Writen(fd, pch, restcnt);
        cnt = 0;
    }

}
/* end for _no_ thread-safe readline */

#else

static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

void thread_readline_destructor(void *ptr)
{
    free(ptr);
#ifdef _TEST
    fprintf(stderr, "%u: released the pointer\n", pthread_self());
#endif
}

void thread_readline_once(void)
{
#ifdef _TEST
    fprintf(stderr, "%u: calling the thread_readline_once\n", pthread_self());
#endif
    pthread_key_create(&rl_key, thread_readline_destructor);
}

static ssize_t thread_my_read(int fd, char *pchUser, Rline *pRline)
{
#define rlcnt pRline->cnt
#define rlbuf pRline->buf
#define rlpchGlobal pRline->pchGlobal

    if (rlcnt <= 0) {
again:
        if ((rlcnt = read(fd, rlbuf, MAXLINE)) < 0) {
            if (errno == EINTR)
                goto again;
            return -1;
        } else if (rlcnt == 0)
            return 0;
        rlpchGlobal = rlbuf;
    }
    rlcnt--;
    *pchUser = *rlpchGlobal++;
    return 1;
}


ssize_t thread_readline(int fd, char *pchUser, ssize_t maxlen)
{
    ssize_t i, rc;
    char ch, *pch;
    Rline *pRline;

    Pthread_once(&rl_once, thread_readline_once);
    if ((pRline = pthread_getspecific(rl_key)) == NULL) {
#ifdef _TEST
        fprintf(stderr, "%u allocating the thread specified data\n", pthread_self());
#endif
        pRline = Calloc(1, sizeof(Rline));
        Pthread_setspecific(rl_key, pRline);
    }
    pch = pchUser;

    for (i = 1; i < maxlen; i++) { /* _must_ start from 1 */
        if ((rc = thread_my_read(fd, &ch, pRline)) == 1) {
            *pch++ = ch;
            if (ch == '\n')
                break;
        } else if (rc == 0) {
            *pch = 0;
            return i-1;
        } else
            return -1;
    }
    *pch = 0;
    return i;
}

ssize_t Thread_readline(int fd, char *ptr, ssize_t maxlen) {
    ssize_t len;
    len = thread_readline(fd, ptr, maxlen);
    if (len < 0)
        err_sys("thread_readline error");
    return len;
}

ssize_t thread_readlinebuf(char **ppchUser)
{
    Rline *pRline;

    Pthread_once(&rl_once, thread_readline_once);
    pRline = pthread_getspecific(rl_key);
    assert(pRline != NULL); /* must _not_ be NULL */
    if (pRline->cnt)
        *ppchUser = pRline->pchGlobal;
    return pRline->cnt;
}

void thread_readlineflush(int fd)
{
    char *pch;
    ssize_t restcnt;
    Rline *pRline;

    Pthread_once(&rl_once, thread_readline_once);
    pRline = pthread_getspecific(rl_key);
    assert(pRline != NULL); /* must _not_ be NULL */

    restcnt = thread_readlinebuf(&pch);
    if (restcnt) {
        Writen(fd, pch, restcnt);
        pRline->cnt = 0;
    }
}


#endif
