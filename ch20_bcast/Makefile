include ../Make.defines

PROGS = udpbcastcli01 udpbcastcli01_err udpbcastcli01_jmp \
		udpbcastcli01_sel udpbcastcli01_ipc udpbcastcli01_batch

all: ${PROGS}

udpbcastcli01: udpbcastcli01.o dg_cli_bcast.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpbcastcli01_err: udpbcastcli01_err.o dg_cli_bcast_err.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpbcastcli01_jmp: udpbcastcli01_jmp.o dg_cli_bcast_jmp.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpbcastcli01_sel: udpbcastcli01_sel.o dg_cli_bcast_sel.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpbcastcli01_ipc: udpbcastcli01_ipc.o dg_cli_bcast_ipc.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpbcastcli01_batch: udpbcastcli01_batch.o dg_cli_bcast_batch.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES}
