
C = /usr/bin/gcc
CFLAGS = -O2
//LIBS = -lrt -lblas -lm
LIBS = -lm -lpthread

all: main2

main2: main2.c
	$(CC) $(CFLAGS) -o main2 main2.c timing.c $(LIBS)

run:
	./main2

plot:
	./main2
	python plotter.py
	convert -delay 5 -loop 0 *.png anim.gif

clean: 
	rm main2
