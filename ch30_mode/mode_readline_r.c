#include "modeserv.h"
#include <assert.h>

struct readval {
    char read_buf[MAXLINE];
    int read_cnt;
    char *read_ptr;
};

static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

static void mode_readline_once(void)
{
    int ret;
    if ((ret = pthread_key_create(&rl_key, free)) != 0) {
        errno = ret;
        perror("pthread_key_create error");
        exit(1);
    }
}

static ssize_t mode_my_read(int fd, char *ptr, struct readval *pval)
{
    if (pval->read_cnt <= 0) { /* initialize */
again:
        if((pval->read_cnt = read(fd, pval->read_buf, sizeof(pval->read_buf))) < 0) {
            if (errno == EAGAIN)
                goto again;
            else
                return -1;
        } else if (pval->read_cnt == 0)
            return 0;
        pval->read_ptr = pval->read_buf; /* pointer to the begin */
    }

    pval->read_cnt--;
    *ptr = *pval->read_ptr++;
    return 1;
}

ssize_t mode_readline_r(int fd, char *vptr, ssize_t maxlen)
{
    struct readval *pval;
    int ret;
    if ((ret = pthread_once(&rl_once, &mode_readline_once)) != 0) {
        errno = ret;
        perror("pthread_once error");
        exit(1);
    }

    if ((pval = pthread_getspecific(rl_key)) == NULL) {
#if (_DEBUG)
        printf("tid %ld will allocate thread data\n", (long)pthread_self());
#endif
        if ((pval = calloc(1, sizeof(struct readval))) == NULL)
            err_sys("calloc error");
        if ((ret = pthread_setspecific(rl_key, pval)) != 0) {
            errno = ret;
            perror("pthread_setspecific error");
            exit(1);
        }
    }


    char *ptr, c;
    int n;
    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((ret = mode_my_read(fd, &c, pval)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (ret == 0) {
            *ptr = 0;
            return n-1;
        } else
            return -1;
    }

    *ptr = 0;
#if (_DEBUG)
    printf("tid (%ld) key (%lu) readline: %s\n", (long)pthread_self(), rl_key, vptr);
#endif
    return n;
}

