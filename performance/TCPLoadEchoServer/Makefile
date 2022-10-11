CC=gcc
CFLAGS=-I.
DEPS = main.h limits.h linkedlist.h userio.h
OBJ = main.o limits.o linkedlist.o userio.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tcploadechoserver: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

all: tcploadechoserver

install:
	cp tcploadechoserver /usr/bin/
