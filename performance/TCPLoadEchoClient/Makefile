CC=gcc
CFLAGS=-I. -lm
DEPS = main.h limits.h userio.h time_functions.h
OBJ = main.o limits.o userio.o time_functions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tcploadechoclient: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

all: tcploadechoclient

install:
	cp tcploadechoclient /usr/bin/
