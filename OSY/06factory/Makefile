CC=gcc
CFLAGS=-Wall -g -O2
LIBS=-lpthread
EXTRA_CFLAGS?=-fsanitize=address -fno-omit-frame-pointer

all: factory

factory: factory.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LIBS) -o $@ $<

test_01:
	make
	./factory < in_01.txt
	make clean
	
test_02:
	make
	./factory < in_02.txt
	make clean

clean:
	rm -f factory

zip:
	tar czf osy06.tgz Makefile *.[ch]
