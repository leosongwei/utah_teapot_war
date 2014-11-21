STD=-std=c90
GL=-lGL -lGLU -lglut
GLC=-lGLC -lXmu -lXext -lX11
CLIB=-lm

CFLAGS=$(STD) $(GL) $(GLC) $(CLIB)

main: a.c
	gcc a.c -o main $(CFLAGS)

clean:
	-rm main
