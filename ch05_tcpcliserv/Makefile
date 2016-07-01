include ../Make.defines

PROGS =	cli_main serv_main cli_main_rst serv_main_rst \
		cli_main_epipe cli_main_redirect write_bin read_bin

all:	${PROGS} cli_main_bin serv_main_bin cli_main_bin_right \
		serv_main_bin_right

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

cli_main_bin: cli_main_bin.o str_cli_bin.o
		${CC} ${CFLAGS} -o $@ cli_main_bin.o str_cli_bin.o ${LIBS}

serv_main_bin: serv_main_bin.o str_echo_bin.o
		${CC} ${CFLAGS} -o $@ serv_main_bin.o str_echo_bin.o ${LIBS}

cli_main_bin_right: cli_main_bin_right.o str_cli_bin_right.o
		${CC} ${CFLAGS} -std=gnu99 -o $@ cli_main_bin_right.o str_cli_bin_right.o ${LIBS}

serv_main_bin_right: serv_main_bin_right.o str_echo_bin_right.o
		${CC} ${CFLAGS} -std=gnu99 -o $@ serv_main_bin_right.o str_echo_bin_right.o ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} \
			cli_main_bin serv_main_bin cli_main_bin_right \
			serv_main_bin_right
