CC=gcc
CFLAGS=-Wall
EXTRA_CFLAGS=

all: nsd forkpipe

forkpipe: forkpipe.o
	$(CC) -o $@ forkpipe.o

nsd: nd.o nsd.o nsd_main.o nd.h nsd.h
	$(CC) -o $@ nd.o nsd.o nsd_main.o nd.h nsd.h

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<

clean:
	rm -f *.o forkpipe nsd

test:
	make
	./forkpipe
	make clean

valgrind:
	make
	valgrind ./forkpipe
	make clean
