include ../Make.defines

PROGS = udpserv03 udpexer01 udpexer01_cli tcpexer01 tcpexer01_cli \
		udpservexer09

all:	${PROGS} udpserv01 udpcli01 udpcli02 udpcli03 \
		udpcli04 udpserv02 udpcli05 udpexer02 udpexer06_cli \
		udpcliexer07 udpcliexer08 udpservexer08

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

udpserv01:	udpserv01.o prac_dg_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpserv02:	udpserv02.o prac_dg_echo02.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcli01:	udpcli01.o prac_dg_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcli02:	udpcli02.o prac_dg_cli02.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcli03:	udpcli03.o prac_dg_cli03.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcli04:	udpcli04.o prac_dg_cli04.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcli05:	udpcli05.o prac_dg_cli05.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpexer02:	udpexer02.o exer02_dg_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpexer06_cli:	udpexer06_cli.o prac_dg_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcliexer07:	udpcliexer07.o exer07_dg_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpcliexer08:	udpcliexer08.o exer08_dg_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

udpservexer08:	udpservexer08.o exer08_dg_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES} \
			udpserv01 udpcli01 udpcli02 udpcli03 \
			udpcli04 udpserv02 udpcli05 udpexer02 udpexer06_cli \
			udpcliexer07 udpcliexer08 udpservexer08
