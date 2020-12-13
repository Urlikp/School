CC=gcc
CFLAGS=-Wall -g -O2
LIBS=-lpthread
EXTRA_CFLAGS?=-fsanitize=address -fno-omit-frame-pointer

all: prod-cons

prod-cons: prod-cons.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f prod-cons

test:
	make
	./prod-cons 4 < in.txt
	make clean

zip:
	tar czf osy05.tgz Makefile *.[ch]
