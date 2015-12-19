STD=-std=gnu99

UNAME=$(shell uname)
ifeq ($(UNAME), Linux)
	GL=-lGL -lGLU -lglut
else ifeq ($(UNAME), Darwin)
	GL=-framework GLUT -framework OpenGL -framework Cocoa
endif

CLIB=-lm

CFLAGS=$(STD) $(GL) $(CLIB) -Wall

main: a.c
	gcc a.c -o main $(CFLAGS)

clean:
	-rm main
