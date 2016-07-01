include ../Make.defines

PROGS = test_gethostbyaddr test_getservbyname test_getservbyport \
		daytimetcpcli01 test_getaddrinfo test_getnameinfo exer01_v01 \
		exer02_v01_mac exer04_v01 exer05_v01 exer06_v01 test_alarm \
		exer11_v01 exer11_v02

all:	${PROGS} test_gethostbyname daytimetcpcli02 daytimetcpserv01 \
		daytimetcpserv01_ipv6 daytimetcpserv02 daytimeudpcli01 daytimeudpserv01 \
		daytimeudpcli02 exer07_v01 exer07_v02 exer08_v01 exer09_v01

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

test_gethostbyname:	test_gethostbyname.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimetcpcli02:	daytimetcpcli02.o my_tcp_connect.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimetcpserv01:	daytimetcpserv01.o my_tcp_listen.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimetcpserv01_ipv6:	daytimetcpserv01_ipv6.o my_tcp_listen_ipv6.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimetcpserv02:	daytimetcpserv02.o my_tcp_listen.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimeudpcli01:	daytimeudpcli01.o my_udp_client.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimeudpserv01:	daytimeudpserv01.o my_udp_server.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

daytimeudpcli02:	daytimeudpcli02.o my_udp_connect.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer07_v01:	exer07_v01.o my_tcp_listen.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer07_v02:	exer07_v02.o my_tcp_listen.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer08_v01:	exer08_v01.o my_tcp_listen.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

exer09_v01:	exer09_v01.o my_udp_server_reuseaddr.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} \
			test_gethostbyname daytimetcpcli02 daytimetcpserv01 \
			daytimetcpserv01_ipv6 daytimetcpserv02 daytimeudpcli01 daytimeudpserv01 \
			daytimeudpcli02 exer07_v01 exer07_v02 exer08_v01 exer09_v01
