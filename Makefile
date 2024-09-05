CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb -I./include/
LIBS=-lm -lGL -lglfw -lGLEW

all: main

main: main.o shader.o
	$(CC) $(CFLAGS) -o main main.o shader.o $(LIBS)

main.o:
	$(CC) $(CFLAGS) -c main.c $(LIBS)

shader.o:
	$(CC) $(CFLAGS) -c shader.c $(LIBS)

clean:
	rm -f main main.o shader.o
