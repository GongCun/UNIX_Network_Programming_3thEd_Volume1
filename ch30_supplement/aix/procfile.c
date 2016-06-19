#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>

#define KA_T_FMT_X "0x%08lx" /* format for printing kernel */

extern void err_sys(const char *str);

extern int Kd; /* KMEM descriptor */

typedef u_longlong_t KA_T; /* kernel memory address type */

static size_t kread(KA_T addr, char *buf, size_t len)
{
	if (lseek64(Kd, (off64_t)addr, L_SET) == (off64_t)-1)
		err_sys("lseek64 error");
	return read(Kd, buf, len);
}

size_t Kread(KA_T addr, char *buf, size_t len)
{
	size_t n;
	if ((n = kread(addr, buf, len)) != len)
		err_sys("kread error");
	return n;
}

int procfile(KA_T fp) {
	struct file file;
	struct socket socket;
	struct inpcb inpcb;
	struct protosw protosw;

	Kread(fp, (char *)&file, sizeof(struct file));
	if (file.f_type != DTYPE_SOCKET)
		return 0;
	else {
		Kread((KA_T)file.f_data, (char *)&socket, sizeof(struct socket));
		printf("Recv-Q: %lu; Send-Q: %lu; "
				" Partial: %d; Incoming: %d\n",
				socket.so_rcv.sb_cc, socket.so_snd.sb_cc, socket.so_q0len, socket.so_qlen);
		Kread((KA_T)socket.so_proto, (char *)&protosw, sizeof(struct protosw));
		if (protosw.pr_protocol == IPPROTO_TCP) {
			Kread((KA_T)socket.so_pcb, (char *)&inpcb, sizeof(struct inpcb));
			printf("inpcb addr = " KA_T_FMT_X "\n", (KA_T)inpcb.inp_ppcb); /* inpcb{} pointer to per-protocol pcb */
			printf("so_pcb addr = " KA_T_FMT_X "\n", (KA_T)socket.so_pcb); /* socket{} pointer to per-protocol pcb */
			printf("lport: %d; fport: %d\n", ntohs(inpcb.inp_lport), ntohs(inpcb.inp_fport));
		}
		return 1;
	}
}


