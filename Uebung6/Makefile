
CC = /usr/bin/gcc
CFLAGS = -O2
LIBS = -lrt -lblas -lm


all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c timing.c $(LIBS)

run:
	./main

