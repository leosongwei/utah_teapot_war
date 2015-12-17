STD=-std=gnu99
GL=-lGL -lGLU -lglut
CLIB=-lm

CFLAGS=$(STD) $(GL) $(CLIB) -Wall

main: a.c
	gcc a.c -o main $(CFLAGS)

clean:
	-rm main
