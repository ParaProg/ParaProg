
C = /usr/bin/gcc
CFLAGS = -O2
//LIBS = -lrt -lblas -lm
LIBS = -lm -lpthread

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c timing.c $(LIBS)

main2: main2.c
	$(CC) $(CFLAGS) -o main2 main2.c timing.c $(LIBS)

run:
	./main

run2:
	./main2

plot:
	python plotter.py
	convert -delay 5 -loop 0 *.png anim.gif

