include ../Make.defines

PROGS = test_recvmsg01

all:	${PROGS} tcp_connect_timeo udp_timeo udp_timeo2 \
		udp_timeo3 my_serv_main tcpclikqueue exer01 exer02 exer04

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

tcp_connect_timeo: tcp_connect_timeo.o my_connect_timeo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udp_timeo: udp_timeo.o my_dg_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udp_timeo2: udp_timeo2.o my_dg_cli2.o my_readable_timeo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udp_timeo3: udp_timeo3.o my_dg_cli3.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

my_serv_main: my_serv_main.o my_str_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

tcpclikqueue: tcpclikqueue.o strclikqueue.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer01: exer01.o exer01_connect_timeo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer02: exer02.o exer02_connect_timeo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer04: exer04.o exer04_str_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} \
			tcp_connect_timeo udp_timeo udp_timeo2 \
			udp_timeo3 my_serv_main tcpclikqueue exer01 exer02 exer04
