#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <sys/types.h>
#include <procinfo.h>
#include "sockq.h"

int Kd; /* KMEM descriptor */

int main(int argc, char *argv[])
{
	pid_t pid;
	static struct FDSINFO *fds;
	static struct PROCINFO pinfo;
	KA_T fp; /* pointer */
	unsigned int nf, i;
	int n;

	if (argc != 2)
		err_quit("Usage: %s <PID>", basename(argv[0]));

	pid = (pid_t)atol(argv[1]);

	if ((Kd = open(KMEM, O_RDONLY, 0)) < 0)
		err_sys("open KMEM error");

	if ((fds = malloc(FDSINFOSIZE)) == NULL)
		err_sys("malloc error");

	if ((n = GETPROCS(&pinfo, PROCSIZE, fds, FDSINFOSIZE, &pid, 1)) != 1) {
		if (n < 0)
			err_sys("GETPROCS error");
		else
			err_quit("Process table is empty");
	}

	nf = pinfo.pi_maxofile;

	for (i = 0; i < nf; i++) {
		fp = fds->pi_ufd[i].fp;
		procfile(fp);
	}

	free(fds);
	return 0;
}

