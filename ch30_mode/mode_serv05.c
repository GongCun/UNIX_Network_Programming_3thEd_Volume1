#include "modeserv.h"
#include <libgen.h>
#include <assert.h>
#include <sys/mman.h>

/* Server: prefork, passing FD */

struct child *cptr;

int nchildren;

static void sig_term(int signo)
{
    printf("process %ld terminated\n", (long) getpid());
    exit(0);
}


pid_t mode_child_make(int i, int listenfd)
{
    int sockfd[2], connfd;
    pid_t pid;
    char c;

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) < 0)
        err_sys("socketpair error");
    if ((pid = Fork()) > 0) { /* parent */
        Close(sockfd[1]);
        cptr[i].child_pid = pid;
        cptr[i].child_pipefd = sockfd[0];
        cptr[i].child_status = 0;
        return pid; /* parent return */
    }

    /* child continue... */
    Close(sockfd[0]);
    Close(listenfd);

    printf("child process %ld starting\n", (long)getpid());
    for (;;) {
        if (mode_read_fd(sockfd[1], &c, 1, &connfd) != 1)
            err_sys("read_fd error");
#if (_DEBUG)
        printf("process %ld read fd %d\n", (long)getpid(), connfd);
#endif
        mode_reply(connfd);	/* process the request */
        Close(connfd);
        Write(sockfd[1], "", 1); /* tell parent we're ready again */
    }
    exit(0);
}

int
main(int argc, char **argv)
{
	int					listenfd, connfd, i, maxfd, navail, nsel;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
	void				sig_int(int);
    fd_set masterset, rset;
    char c;

    if (argc != 3)
        err_quit("Usage: %s <port> <#children>", basename(argv[0]));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
    nchildren = atoi(argv[2]);

    if ((cptr = calloc(nchildren, sizeof(struct child))) == NULL)
        err_sys("calloc error");

    Signal(SIGTERM, sig_term);

    FD_ZERO(&masterset);
    FD_SET(listenfd, &masterset);
    maxfd = listenfd;
    navail = nchildren;


	for (i = 0; i < nchildren; i++) {
        mode_child_make(i, listenfd);
        FD_SET(cptr[i].child_pipefd, &masterset);
        maxfd = max(maxfd, cptr[i].child_pipefd);
    }
    Signal(SIGINT, sig_int);
    for (;;) {
        rset = masterset; /* keep listenfd and child pipefd */
        if (navail <= 0)
            FD_CLR(listenfd, &rset);
        if ((nsel = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            else
                err_sys("select error");
        }
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
            /* printf("connected client port: %d\n", ntohs(cliaddr.sin_port));
             * */
            for (i = 0; i < nchildren; i++) {
                if (cptr[i].child_status == 0)
                    break;
            }
            if (i == nchildren)
                err_quit("no available children");
            cptr[i].child_status = 1;
            cptr[i].child_count++;
            navail--;
            mode_write_fd(cptr[i].child_pipefd, &c, 1, connfd);
#if (_DEBUG)
            printf("write process %ld fd %d\n", (long)cptr[i].child_pid, connfd);
            /* the number of fd which parent sent, is probably different from
             * that child received, see APUE section 17.4 */
#endif
            Close(connfd);
            if (--nsel == 0)
                continue; /* no children return,
                             all done with select() result */
        }
        for (i = 0; i < nchildren; i++) {
            if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
                if (Read(cptr[i].child_pipefd, &c, 1) <= 0) {
                    err_msg("child %d terminated unexpected\n", i);
                    if (cptr[i].child_status == 0) {
                        navail--;
                        cptr[i].child_status = 1;
                    }
                    FD_CLR(cptr[i].child_pipefd, &masterset);
                } else {
                    cptr[i].child_status = 0;
                    navail++;
                }
                if (--nsel == 0)
                    break; /* all done */
            }
        }
    }

    exit(0);
}
