include ../Make.defines

PROGS = exer05

all:	${PROGS} udpmcastserv01 udpmcastcli01

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

udpmcastserv01: udpmcastserv01.o udp_server_ip4.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpmcastcli01: udpmcastcli01.o dg_cli_mcast.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} udpmcastserv01 udpmcastcli01
