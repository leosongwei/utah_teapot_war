CFLAGS=-lGL -lGLU -lglut -lm -std=gnu99

main: a.c
	gcc a.c -o main $(CFLAGS)

clean:
	-rm main
