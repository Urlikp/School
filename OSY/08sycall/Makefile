CC=gcc
CFLAGS=-nostdlib -nostdinc -static -m32 -Wall -g -O2

all: hexconv

hexconv: hexconv.c
	$(CC) $(CFLAGS) -o $@ $<

test:
	make
	./hexconv
	make clean

clean:
	rm -f hexconv

zip:
	tar czf osy07.tgz *.c
