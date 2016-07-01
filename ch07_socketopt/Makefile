include ../Make.defines

PROGS =	getbufsiz getbufmss cli_main_rst cli_main_bind \
		cli_main_bind_rst

all:	${PROGS} cli_main_write2 cli_main_nodelay \
		cli_main_keepalive

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

cli_main_write2:	cli_main_write2.o str_cli_write2.o
		${CC} ${CFLAGS} -o $@ cli_main_write2.o str_cli_write2.o ${LIBS}

cli_main_nodelay:	cli_main_nodelay.o str_cli_write2.o
		${CC} ${CFLAGS} -o $@ cli_main_nodelay.o str_cli_write2.o ${LIBS}

cli_main_keepalive:	cli_main_keepalive.o str_cli_select.o
		${CC} ${CFLAGS} -o $@ cli_main_keepalive.o str_cli_select.o ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} \
			cli_main_write2 cli_main_nodelay \
			cli_main_keepalive
