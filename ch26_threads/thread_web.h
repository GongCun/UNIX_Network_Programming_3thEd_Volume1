#pragma once
#include "unpthread_ex.h"

#define MAXFILES 30
extern const int WEBSERVPORT;
extern pthread_mutex_t ndone_mutex;
extern pthread_cond_t ndone_cond;
extern int ndone;

struct file {
    char *f_pchName;
    char *f_pchHost;
    int f_iFd;
    int f_iFlags;
    pthread_t f_thrId;
} file[MAXFILES];

#define F_CONNECTING 1 /* connect() in progress */
#define F_READING 2 /* connect() complete; now reading */
#define F_DONE 4 /* all done */
#define F_JOINED 8 /* main has pthread_join'ed */
#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

int nconn, nfiles, nlefttoconn, nlefttoread;

void *thread_do_get_read(void *);
void thread_home_page(const char *, const char *, char **);
void thread_write_get_cmd(struct file *);

