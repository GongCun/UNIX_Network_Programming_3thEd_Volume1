#include "thread_web.h"
#include <string.h>

static int host_connect(char *, int);

void *thread_do_get_read(void *pv)
{
    int fd, wfd, n;
    char line[MAXLINE], *filename;
    struct file *pf;

    pf = (struct file *)pv;

    fd = host_connect(pf->f_pchHost, WEBSERVPORT);
    if (fd < 0)
        err_sys("host_connect error");
    pf->f_iFd = fd;
    pf->f_thrId = pthread_self();

    printf("thread_do_get_read for %s, fd %d, thread %u\n",
            pf->f_pchName, pf->f_iFd, pf->f_thrId);

    thread_write_get_cmd(pf);

    filename = strrchr(pf->f_pchName, '/');
    if (filename == NULL)
        filename = pf->f_pchName;
    else
        filename++;
    if ((wfd = open(filename, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
        err_sys("open error");

    /* read server's reply */
    for (;;) {
        if ((n = Read(pf->f_iFd, line, sizeof(line))) == 0)
            break; /* server closed connection */
        Writen(wfd, line, n);
        printf("read %d bytes from %s\n", n, pf->f_pchName);
    }
    printf("end-of-file on %s\n", pf->f_pchName);
    Close(pf->f_iFd);
    Close(wfd);
    pf->f_iFlags = F_DONE;

    Pthread_mutex_lock(&ndone_mutex);
    ndone++;
    Pthread_cond_signal(&ndone_cond);
    Pthread_mutex_unlock(&ndone_mutex);
    return pf;
}

static int host_connect(char *hostaddr, int servport)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servport);
    servaddr.sin_addr = (*(struct in_addr *)hostaddr);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr))
            < 0)
        return -1;

    return sockfd;
}

void thread_write_get_cmd(struct file *pf)
{
    char line[MAXLINE];
    int n;

    n = snprintf(line, sizeof(line), GET_CMD, pf->f_pchName);
    Writen(pf->f_iFd, line, n);
    printf("wrote %d bytes for %s\n", n, pf->f_pchName);
    pf->f_iFlags = F_READING;
}

void thread_home_page(const char *webName, const char *homepage, char **ppchReturn)
{
    struct hostent *ph;
    if ((ph = gethostbyname(webName)) == NULL)
        err_sys("gethostbyname error for host: %s", webName);

    char **ppch, *pchMalloc, line[MAXLINE];
    int n, fd;
    if (ph->h_addrtype != AF_INET)
        err_sys("unknown address type");
    for (ppch = ph->h_addr_list; *ppch != NULL; ppch++) {
        if((fd = host_connect(*ppch, WEBSERVPORT)) < 0)
            continue;
        break;
    }
    if (*ppch == NULL)
        err_quit("can't connect to web server");

    pchMalloc = malloc(ph->h_length); /* 32-bit IPv4 address */
    if (pchMalloc == NULL)
        err_sys("malloc error");
    memcpy(pchMalloc, *ppch, ph->h_length);
    *ppchReturn = pchMalloc;

    n = snprintf(line, sizeof(line), GET_CMD, homepage);
    Writen(fd, line, n);
    for (;;) {
        if ((n = Read(fd, line, sizeof(line))) == 0)
            break;
        printf("read %d bytes of home page\n", n);
        /* get the ip and filename with the data 
         * for thread work */
    }
    printf("end-of-file on home page\n");
    Close(fd);
}



