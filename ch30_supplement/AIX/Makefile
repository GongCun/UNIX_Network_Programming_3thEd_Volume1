#CC = gcc
PROGS = sockq

OBJ = sockq.o procfile.o error.o

all: $(PROGS)

$(PROGS): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

error.c:
	ln -s -f ../error.c .

clean:
	rm -f $(PROGS) *.o

