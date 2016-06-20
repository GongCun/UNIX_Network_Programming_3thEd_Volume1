include ../Make.defines

PROGS = oob_cli01 oob_serv01 oob_serv02_err oob_serv02 \
		oob_cli02 oob_serv03 oob_cli02_no_oob oob_cli03 \
		oob_serv04 oob_serv04_v2 oob_cli04 oob_serv05 oob_serv_poll

all:	${PROGS} oob_hb_cli01 oob_hb_serv01

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

oob_hb_cli01: oob_hb_cli01.o oob_heartbeat_cli.o oob_str_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

oob_hb_serv01: oob_hb_serv01.o oob_heartbeat_serv.o oob_str_echo.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} oob_hb_cli01 oob_hb_serv01
