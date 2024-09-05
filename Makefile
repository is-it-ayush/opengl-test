CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb -I./include/
LIBS=-lm -lGL -lglfw -lGLEW

all: main

main: main.o shader.o callback.o
	$(CC) $(CFLAGS) -o main main.o shader.o callback.o $(LIBS)

main.o:
	$(CC) $(CFLAGS) -c main.c $(LIBS)

shader.o:
	$(CC) $(CFLAGS) -c ./src/shader.c $(LIBS)

callback.o:
	$(CC) $(CFLAGS) -c ./src/callback.c $(LIBS)

clean:
	rm -f main main.o shader.o callback.o
