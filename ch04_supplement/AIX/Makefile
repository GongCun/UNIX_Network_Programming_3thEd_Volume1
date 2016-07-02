#CC = gcc
PROGS = sockq

OBJ = sockq.o procfile.o error.o

all: $(PROGS)

$(PROGS): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Must separate compile the procfile.c and sockq.c
# because the -D_KERNEL will make function getprocs()
# not work.
$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

error.c:
	ln -s -f ../error.c .

clean:
	rm -f $(PROGS) *.o

