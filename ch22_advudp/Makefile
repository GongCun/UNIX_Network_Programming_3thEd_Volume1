include ../Make.defines

PROGS = 

all:	${PROGS} adv_udpserv01 adv_udpcli01 test_getifaddrs adv_udpserv02

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

adv_udpserv01:	adv_udpserv01.o adv_dg_echo.o adv_recvfrom_flags.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

adv_udpcli01:	adv_udpcli01.o adv_dg_cli.o adv_dg_send_recv.o rtt_lib.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

test_getifaddrs: test_getifaddrs.o adv_inet_masktop.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

adv_udpserv02: adv_udpserv02.o adv_mydg_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES} adv_udpserv01 adv_udpcli01 test_getifaddrs adv_udpserv02
