#include "myweb.h"

static void sig_chld(int signo)
{
    int i;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        nlefttoread--;
        printf("child %d terminated\n", pid);
        for (i = 0; i < nfiles; i++) 
            if (file[i].f_flags == 0)
                break;
        if (i != nfiles) {
            if (Fork() ==0) {
                printf("child %d started\n", getpid());
                home_page(file[i].f_host, file[i].f_name);
                exit(0);
            }
            file[i].f_flags = F_DONE;
        }
    }
    return;
}


int main(int argc, char *argv[])
{
    int i, fd, n, maxnconn, flags, error;
    char buf[MAXLINE];
    fd_set rs, ws;
    if (argc < 5)
        err_quit("usage: web <#conn> <hostname> <homepage> <file1> ...");
    maxnconn = atoi(argv[1]);
    nfiles = min(argc - 4, MAXFILES);
    for (i = 0; i < nfiles; i++) {
        file[i].f_name = argv[i+4];
        file[i].f_host = argv[2];
        file[i].f_flags = 0;
    }
    printf("nfiles = %d\n", nfiles);

    home_page(argv[2], argv[3]);

    nlefttoread = nlefttoconn = nfiles;
    nconn = 0;

    Signal(SIGCHLD, sig_chld);

    for (i = 0; i < maxnconn; i++) {
        if (Fork() == 0) {
            /*child */
            printf("child %d started\n", getpid());
            home_page(file[i].f_host, file[i].f_name);
            exit(0);
        }
        file[i].f_flags = F_DONE;
    }

    while (nlefttoread > 0) {
        pause();
    }
    printf("all finished\n");
    return(0);
}

