include ../Make.defines

# Must specify the libnet path
# LIBNETROOT := /Users/gongcunjust/Develop/libnet
LIBNETINC := ${LIBNETROOT}/include
LIBNETLIB := ${LIBNETROOT}/src
LIBNETLIBS := ${LIBNETROOT}/src/libnet.a

# Must specify the libpcap path
# LIBPCAPROOT := /Users/gongcunjust/Develop/libpcap-1.1.1
LIBPCAPINC := ${LIBPCAPROOT}
LIBPCAPLIB := ${LIBPCAPROOT}
LIBPCAPLIBS := ${LIBPCAPROOT}/libpcap.a

PROGS =	dlt_sniff dlt_test_dns dlt_udpcksum dlt_test_tcp dlt_tcp_dns 

all:	${PROGS}

dlt_sniff:	./dlt_sniff.c
		${CC} ${CFLAGS} -O -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -I. -I${LIBPCAPINC} -L${LIBPCAPLIB} -o $@ $< ${LIBPCAPLIBS} ${LIBS}

dlt_test_dns:	./dlt_test_dns.c ./dlt_udp_write.c ./dlt_send_dnsquery.c ./dlt_cksum.c ./dlt_open.c
		${CC} ${CFLAGS} -O -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -D_DEBUG -I. -I${LIBPCAPINC} -L${LIBPCAPLIB} -o $@ $^ ${LIBPCAPLIBS} ${LIBS}

dlt_udpcksum:	./dlt_udpcksum.c ./dlt_udp_write.c ./dlt_send_dnsquery.c ./dlt_cksum.c ./dlt_open.c ./dlt_open_pcap.c ./dlt_test_udp.c ./dlt_udp_read.c
		${CC} ${CFLAGS} -O -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -D_DEBUG -I. -I${LIBPCAPINC} -L${LIBPCAPLIB} -o $@ $^ ${LIBPCAPLIBS} ${LIBS}

dlt_test_tcp: dlt_test_tcp.c dlt_cksum.c
		${CC} ${CFLAGS} -O -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -D_DEBUG -I. -I${LIBPCAPINC} -L${LIBPCAPLIB} -o $@ $^ ${LIBPCAPLIBS} ${LIBS}

dlt_tcp_dns: dlt_tcp_dns.c dlt_cksum.c
		${CC} ${CFLAGS} -O -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -D_DEBUG -I. -I${LIBPCAPINC} -L${LIBPCAPLIB} -o $@ $^ ${LIBPCAPLIBS} ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
		rm -r -f *.dSYM
