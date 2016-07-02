#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <arpa/inet.h>
#include "sockq.h"

void process_fds(pid_t, uint32_t);

int main(int argc, char *argv[])
{
    pid_t pid;
    struct proc_taskallinfo tai;
    int nb;

    if (argc != 2)
        err_quit("Usage: %s <PID>", basename(argv[0]));

    pid = (pid_t)atol(argv[1]);
    if ((nb = proc_pidinfo(pid, PROC_PIDTASKALLINFO, 0, &tai, sizeof(tai))) < 0)
        err_sys("proc_pidinfo error");
    else if (nb < sizeof(struct proc_taskallinfo)) {
        err_quit("proc_pidinfo fail: too few bytes: "
                "expected %ld, got %ld",
                sizeof(struct proc_taskallinfo), nb);
    }

    process_fds(pid, tai.pbsd.pbi_nfiles);

    return 0;
}

void process_fds(pid_t pid, uint32_t nfiles)
{
    struct proc_fdinfo *Nfdp;
    struct proc_fdinfo *fdp;
    struct socket_fdinfo si;
    int i, nb, nf;

    if ((Nfdp = malloc(sizeof(struct proc_fdinfo) * nfiles)) == NULL)
        err_sys("malloc error");

    if ((nb = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, Nfdp, sizeof(struct proc_fdinfo) * nfiles)) < 0)
        err_sys("proc_pidinfo error");
    nf = (int) (nb / sizeof(struct proc_fdinfo));

    for (i = 0; i < nf; i++) {
        fdp = Nfdp + i;
        if (fdp->proc_fdtype == PROX_FDTYPE_SOCKET) {
            nb = proc_pidfdinfo(pid, fdp->proc_fd, PROC_PIDFDSOCKETINFO, &si, sizeof(si));
            if (nb < 0)
                err_sys("proc_pidfdinfo error");
            else if (nb < sizeof(si)) {
                err_quit("proc_pidfdinfo fail: too few bytes: "
                        "expected: %ld, got: %ld\n",
                        sizeof(si), nb);
            }
            if (si.psi.soi_kind == SOCKINFO_TCP) {
                printf("Local-Port: %d; Foreign-Port: %d\n"
                        "Recv-Q: %d; Send-Q: %d\n"
                        "Partial-Q: %hd; Incoming-Q: %hd\n",
                        ntohs(si.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport),
                        ntohs(si.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport),
                        si.psi.soi_rcv.sbi_cc,
                        si.psi.soi_snd.sbi_cc,
                        si.psi.soi_qlen, si.psi.soi_incqlen);
            }
        }
    }
    return;
}





