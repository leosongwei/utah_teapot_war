STD=-std=gnu99
GL=-lGL -lGLU -lglut
CLIB=-lm

CFLAGS=$(STD) $(GL) $(CLIB)

main: a.c
	gcc a.c -o main $(CFLAGS)

clean:
	-rm main
