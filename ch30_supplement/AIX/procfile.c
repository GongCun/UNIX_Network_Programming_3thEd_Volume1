#include <stdlib.h>
#include <stdio.h>
#define _KERNEL 1
#include <sys/file.h>
#undef _KERNEL
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>
#include "sockq.h"

static size_t kread(KA_T addr, char *buf, size_t len)
{
	if (LSEEK(Kd, (OFF_T)addr, L_SET) == (OFF_T)-1)
		err_sys("LSEEK error");
	return read(Kd, buf, len);
}

size_t Kread(KA_T addr, char *buf, size_t len)
{
	size_t n;
	if ((n = kread(addr, buf, len)) != len)
		err_sys("kread error");
	return n;
}

void procfile(KA_T fp) {
	struct file file;
	struct socket socket;
	struct inpcb inpcb;
	struct protosw protosw;

	Kread(fp, (char *)&file, sizeof(struct file));
	if (file.f_type == DTYPE_SOCKET) {
		Kread((KA_T)file.f_data, (char *)&socket, sizeof(struct socket));
		Kread((KA_T)socket.so_proto, (char *)&protosw, sizeof(struct protosw));
		if (protosw.pr_protocol == IPPROTO_TCP) {
			Kread((KA_T)socket.so_pcb, (char *)&inpcb, sizeof(struct inpcb));
			printf("Local-Port: %d; Foreign-Port: %d\n"
					"Recv-Q: %lu; Send-Q: %lu\n"
					"Partial-Q: %d; Incoming-Q: %d\n",
					ntohs(inpcb.inp_lport), ntohs(inpcb.inp_fport),
					socket.so_rcv.sb_cc, socket.so_snd.sb_cc,
					socket.so_q0len, socket.so_qlen);
		}
	}
	return;
}
