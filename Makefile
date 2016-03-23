STD=-std=gnu99

UNAME=$(shell uname)
ifeq ($(UNAME), Linux)
	HEADER=
	GL=-lGL -lGLU -lglut
else ifeq ($(UNAME), Darwin)
	GL=-framework GLUT -framework OpenGL -framework Cocoa
	HEADER=-I/opt/X11/include/
endif

CLIB=-lm

CFLAGS=$(STD) $(GL) $(CLIB) $(HEADER) -lgc -Wall

main: a.c
	gcc a.c -o main $(CFLAGS)

clean:
	-rm main
