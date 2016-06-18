#include "thread_web.h"
#include <libgen.h>
#include <assert.h>

const int WEBSERVPORT = 80;
int ndone = 0;
pthread_mutex_t ndone_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ndone_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
    int i, maxconn, nfiles, nlefttoread, nlefttoconn, nconn;
    char *pchServaddr, *pchWeb, *pchHomepage;
    struct file *pf;
    pthread_t tid;

    if (argc < 5)
        err_quit("Usage: %s <#conns> <webName> <homepage> file1 ...", basename(argv[0]));
    maxconn = atoi(argv[1]);
    pchWeb = argv[2];
    pchHomepage = argv[3];
    nfiles = nlefttoread = nlefttoconn = argc-4;
    nconn = 0;

    thread_home_page(pchWeb, pchHomepage, &pchServaddr);
    /* don't forget free(pchServaddr); */

#ifdef _TEST
    char buff[MAXLINE];
    if (inet_ntop(AF_INET, pchServaddr, buff, sizeof(buff)) == NULL)
        err_sys("inet_ntop error");
    printf("web address is %s\n", buff);

    printf("\nnfiles = %d\n", nfiles);

    for (i = 0; i < nfiles; i++) {
        file[i].f_pchName = argv[i+4];
        file[i].f_pchHost = pchServaddr;
        file[i].f_iFlags = 0;
        Pthread_create(&tid, NULL, thread_do_get_read, &file[i]);
    }

    /* wait for the thread exit */
    printf("sleeping for wait thread\n");
    sleep(10);

#else
    printf("\nnfiles = %d\n", nfiles);
    for (i = 0; i < nfiles; i++) {
        file[i].f_pchName = argv[i+4];
        file[i].f_pchHost = pchServaddr;
        file[i].f_iFlags = 0;
    }

    while (nlefttoread > 0) {
        while (nconn < maxconn && nlefttoconn > 0) {
            /* find a file to read */
            for (i = 0; i < nfiles; i++)
                if (file[i].f_iFlags == 0)
                    break;
            assert(i < nfiles);
            file[i].f_iFlags = F_CONNECTING;
            Pthread_create(&tid, NULL, thread_do_get_read, &file[i]);
            nconn++;
            nlefttoconn--;
        }
        /* wait for any thread exit */
        Pthread_mutex_lock(&ndone_mutex);
        while (ndone == 0)
            Pthread_cond_wait(&ndone_cond, &ndone_mutex);
        for (i = 0; i < nfiles; i++) {
            if (file[i].f_iFlags == F_DONE) {
                Pthread_join(file[i].f_thrId, (void **)&pf);
                assert(&file[i] == pf);
                pf->f_iFlags = F_JOINED;
                ndone--;
                nconn--;
                nlefttoread--;
                printf("thread %u for %s done\n", pf->f_thrId, pf->f_pchName);
            }
        }
        Pthread_mutex_unlock(&ndone_mutex);
    }
#endif

    free(pchServaddr);

    return 0;
}

