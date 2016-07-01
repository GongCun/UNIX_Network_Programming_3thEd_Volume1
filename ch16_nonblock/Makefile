include ../Make.defines

PROGS =	tcpcli_rst tcpserv_rst tcpserv_nonb

all:	${PROGS} test_gf_time tcpclinonb tcpclinonb02 \
		tcpclinonb03 tcpclinonbConn web web2

$(PROGS):   %: %.o
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

tcpclinonb:	tcpclinonb.o strclinonb.o gf_time.o
		${CC} ${CFLAGS} -o $@ tcpclinonb.o strclinonb.o gf_time.o ${LIBS}

tcpclinonb02:	tcpclinonb.o strclinonb02.o
		${CC} ${CFLAGS} -o $@ tcpclinonb.o strclinonb02.o ${LIBS}

tcpclinonb03:	tcpclinonb.o strclinonb03.o
		${CC} ${CFLAGS} -o $@ tcpclinonb.o strclinonb03.o ${LIBS}

tcpclinonbConn:	tcpclinonbConn.o connect_nonb.o strclinonb03.o
		${CC} ${CFLAGS} -o $@ tcpclinonbConn.o connect_nonb.o strclinonb03.o ${LIBS}

test_gf_time:	test_gf_time.o gf_time.o
		${CC} ${CFLAGS} -o $@ test_gf_time.o gf_time.o ${LIBS}

web:	web.o home_page.o start_connect.o write_get_cmd.o
		${CC} ${CFLAGS} -o $@ web.o home_page.o start_connect.o \
			write_get_cmd.o ${LIBS}

web2:	web2.o home_page.o start_connect.o write_get_cmd.o
		${CC} ${CFLAGS} -o $@ web2.o home_page.o start_connect.o \
			write_get_cmd.o ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} \
			test_gf_time tcpclinonb tcpclinonb02 \
			tcpclinonb03 tcpclinonbConn web web2
