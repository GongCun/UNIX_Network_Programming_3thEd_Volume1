#include	"myadvio.h"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

void
str_cli(FILE * fp, int sockfd)
{
	int		kq, ready, isfile;
	char		buf       [MAXLINE];
	int		i         , n;

	static int	stdineof = 0;
	struct kevent	kev[2];
	struct timespec	ts;
    struct stat st;

    printf("using kqueue\n");

	isfile = (fstat(fileno(fp), &st) == 0 &&
		  (st.st_mode & S_IFMT) == S_IFREG);
	EV_SET(&kev[0], fileno(fp), EVFILT_READ, EV_ADD, 0, 0, NULL);
	EV_SET(&kev[1], sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if ((kq = kqueue()) < 0)
		err_sys("kqueue error");

	ts.tv_sec = ts.tv_nsec = 0;
	if (kevent(kq, kev, 2, NULL, 0, &ts) < 0)
		err_sys("kevent error");

	for (;;) {
		ready = kevent(kq, NULL, 0, kev, 2, NULL);
		if (ready < 0)
			err_sys("kevent error");
		for (i = 0; i < ready; i++) {
			if (kev[i].ident == sockfd) {

				if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
					if (stdineof == 1)
						return;	/* normal termination */
					else
						err_quit("str_cli: server terminated prematurely");
				}
				Write(fileno(stdout), buf, n);
			}
			if (kev[i].ident == fileno(fp)) {
				n = Read(fileno(fp), buf, MAXLINE);
				if (n == 0 || (isfile && n == kev[i].data)) {
					stdineof = 1;
					Shutdown(sockfd, SHUT_WR);	/* send FIN */
					kev[i].flags = EV_DELETE;
					if (kevent(kq, &kev[i], 1, NULL, 0, &ts) < 0)
						err_sys("kevent error");
					continue;
				}
				Writen(sockfd, buf, n);
			}
		}
	}
}
