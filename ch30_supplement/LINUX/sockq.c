#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include "sockq.h"

#define PROCDIR "/proc/%s/fd"
#define PROCTCP "/proc/net/tcp"
#define WHITE " \t\n"
#define FIELDS 32 /* fields# */
#define BUFSIZE 256

void sockq(unsigned long long);
static void split_colon(char *, char *, char *);

int main(int argc, char *argv[])
{
    DIR *dp;
    char *ch, buf[BUFSIZE], filename[BUFSIZE], pathname[BUFSIZE];
    struct dirent *dirp;
    int n;
    unsigned long long ino;
    struct stat statbuf;

    if (argc != 2)
        err_quit("Usage: %s <PID>", basename(argv[0]));

    snprintf(buf, sizeof(buf), PROCDIR, argv[1]);
    if ((dp = opendir(buf)) == NULL)
        err_sys("opendir error");

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 ||
                strcmp(dirp->d_name, "..") == 0)
            continue;
        snprintf(filename, sizeof(filename), "%s/%s", buf, dirp->d_name);

        if ((n = readlink(filename, pathname, sizeof(pathname)-1)) == -1)
            err_sys("readlink");
        pathname[n] = '\0';
        if (*pathname == '/')
            continue;
        if ((ch = strchr(pathname, ':'))) {
            *ch = '\0';
            if (strcmp(pathname, "socket") == 0) {
                if (stat(filename, &statbuf) < 0)
                    err_sys("stat error");
                ino = (unsigned long long)statbuf.st_ino;
                sockq(ino);
            }
        }
    }

    return 0;
}

void sockq(unsigned long long ino)
{
    int i;
    unsigned long txq, rxq;
    int lport, fport;
    char buf[1024], *array[FIELDS], *end, ptxq[16], prxq[16], plport[8], pfport[8], addrbuf[32];
    FILE *fp;
    unsigned long long procino;

    if ((fp = fopen(PROCTCP, "r")) == NULL)
        err_sys("fopen error");

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        buf[strlen(buf)-1] = '\0';
        if ((array[0] = strtok(buf, WHITE)) == NULL)
            err_quit("strtok error");
        for (i = 1; i < FIELDS; i++) {
            if ((array[i] = strtok(NULL, WHITE)) == NULL)
                break;
        }
        if (i == 12) continue; /* header */
        end = (char *)NULL;
        if (!array[9] || !*array[9] ||
                (procino = strtoull(array[9], &end, 0)) == ULONG_MAX ||
                !end || *end)
            err_quit("read ino error");
        if (procino == ino) {
            /* Get local port */
            split_colon(array[1], addrbuf, plport);
            end = (char *)NULL;
            if ((lport = (int) strtol(plport, &end, 16)) == ULONG_MAX ||
                    !end || *end)
                err_quit("read lport size error");

            /* Get foreign port */
            split_colon(array[2], addrbuf, pfport);
            end = (char *)NULL;
            if ((fport = (int) strtol(pfport, &end, 16)) == ULONG_MAX ||
                    !end || *end)
                err_quit("read fport size error");

            /* Get queue depth */
            split_colon(array[4], ptxq, prxq);
            end = (char *)NULL;
            if ((txq = strtoul(ptxq, &end, 16)) == ULONG_MAX ||
                    !end || *end)
                err_quit("read rcv size error");
            end = (char *)NULL;
            if ((rxq = strtoul(prxq, &end, 16)) == ULONG_MAX ||
                    !end || *end)
                err_quit("read snt size error");

            printf("Local-Port: %d; Foreign-Port: %d\n"
                    "Recv-Q: %lu; Send-Q: %lu\n",
                    lport, fport, rxq, txq);
        }
    }

    if (ferror(fp))
        err_quit("read proc file error");
    if (fclose(fp) == EOF)
        err_sys("fclose error");

    return;
}

void split_colon(char *str, char *s1, char *s2)
{
    char *ptr;
    int n;

    if ((ptr = strchr(str, ':')) == NULL)
        err_quit("read queue error");
    n = ptr - str;
    strncpy(s1, str, n);
    s1[n] = '\0';
    n = strlen(str) - strlen(s1) -1;
    strncpy(s2, ptr+1, n);
    s2[n] = '\0';

    return;
}
